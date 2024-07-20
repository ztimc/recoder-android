//
// Created by PC on 2024/2/4.
//

#include <memory>
#include <Oscillator.h>

#include "RecorderOboeEngine.h"

RecorderOboeEngine::RecorderOboeEngine(JavaVM *javaVM, jobject jObj) {
    mFileEncoder = std::make_shared<AudioFileEncoder>();
    mJavaVM = javaVM;
    mJObj = jObj;
}

oboe::Result RecorderOboeEngine::start(
        int deviceId,
        int channelCount,
        int sampleRate,
        RecorderCodec codec,
        oboe::AudioFormat audioFormat,
        const char *filePath) {
    mDeviceId = deviceId;
    mSampleRate = sampleRate;
    mChannelCount = channelCount;
    mAudioFormat = audioFormat;
    mCodec = codec;

    initFile(filePath);

    std::lock_guard<std::mutex> lock(mLock);
    oboe::Result result;

    int tryCount = 0;
    do {
        if (tryCount > 0) {
            usleep(20 * 1000); // Sleep between tries to give the system time to settle.
        }
        result = openStream();
        if (result == oboe::Result::OK) {

            stopThread = false;
            taskThread = std::thread(&RecorderOboeEngine::processTasks, this);

            LOGD("Stream opened: AudioAPI = %d, channelCount = %d, deviceID = %d",
                 static_cast<int32_t>(mStream->getAudioApi()),
                 mStream->getChannelCount(),
                 mStream->getDeviceId());

            int32_t maxFrames = mStream->getBufferCapacityInFrames();
            int32_t inputBufferSize = maxFrames * mStream->getChannelCount();
            mInputConverter = std::make_unique<FormatConverterBox>(inputBufferSize,
                                                                   mStream->getChannelCount(),
                                                                   mStream->getFormat(),
                                                                   oboe::AudioFormat::Float);

            result = mStream->requestStart();
            if (result != oboe::Result::OK) {
                LOGE("Error starting recorder stream. Error: %s", oboe::convertToText(result));
                mStream->close();
                mStream.reset();
            }
        } else {
            LOGE("Error creating playback stream. Error: %s", oboe::convertToText(result));
        }
    } while (result != oboe::Result::OK && tryCount++ < 3);
    return result;
}

oboe::Result RecorderOboeEngine::stop() {
    oboe::Result result = oboe::Result::OK;
    // Stop, close and delete in case not already closed.
    std::lock_guard<std::mutex> lock(mLock);


    if (mStream) {
        result = mStream->stop();
        mStream->close();
        mStream.reset();
        mFileEncoder->closeFile();
    }

    {
        std::lock_guard<std::mutex> lk(taskQueueMutex);
        stopThread = true;
    }

    taskCondition.notify_one();
    taskThread.join();

    return result;
}

void RecorderOboeEngine::initFile(const char *filePath) {


    mFileEncoder->openAudioFile(
            filePath,
            mChannelCount,
            mSampleRate,
            mAudioFormat,
            mCodec);
}


oboe::Result RecorderOboeEngine::openStream() {
    oboe::AudioStreamBuilder builder;
    oboe::Result result = builder.setSharingMode(oboe::SharingMode::Exclusive)
            ->setDirection(oboe::Direction::Input)
            ->setInputPreset(oboe::InputPreset::Camcorder)
            ->setPerformanceMode(oboe::PerformanceMode::PowerSaving)
            ->setFormatConversionAllowed(true)
            ->setSampleRate(mSampleRate)
            ->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Best)
            ->setCallback(this)
            ->setAudioApi(oboe::AudioApi::Unspecified)
            ->setFormat(mAudioFormat)
            ->setChannelCount(mChannelCount)
            ->setDeviceId(mDeviceId)
            ->openStream(mStream);
    if (result == oboe::Result::OK) {
        mChannelCount = mStream->getChannelCount();
    }
    return result;
}

oboe::DataCallbackResult
RecorderOboeEngine::onAudioReady(oboe::AudioStream *oboeStream, void *audioData,
                                 int32_t numFrames) {
    if (isPause) {
        return oboe::DataCallbackResult::Continue;
    }

    int32_t writeLen = mFileEncoder->writeToFile(audioData, numFrames);
    if (writeLen < 0) {
        LOGE("Error writeToFile. Error: %s", "writeLen < 0");
        return oboe::DataCallbackResult::Stop;
    }
    {

        std::lock_guard<std::mutex> lock(taskQueueMutex);
        taskQueue.emplace([this, numFrames, audioData]() {
            int size = numFrames * mChannelCount;
            float out[size];
            float mono[numFrames];
            mInputConverter->convert(out, numFrames, audioData);
            if (mChannelCount == oboe::Stereo) {
                for (int32_t i = 0; i < numFrames; ++i) {
                    float left = out[2 * i];
                    float right = out[2 * i + 1];
                    mono[i] = (left + right) / 2.0f;  // Mixing to mono
                }
            }

            JNIEnv *env;
            mJavaVM->AttachCurrentThread(&env, nullptr);
            jclass cls = env->GetObjectClass(mJObj);
            jmethodID method = env->GetMethodID(cls, "onAudioData", "([F)V");
            jfloatArray jData = env->NewFloatArray(numFrames);
            env->SetFloatArrayRegion(jData,
                                     0,
                                     numFrames,
                                     mChannelCount == oboe::Stereo ? mono : out);
            env->CallVoidMethod(mJObj, method, jData);
            env->DeleteLocalRef(jData);
            mJavaVM->DetachCurrentThread();
        });
    }
    taskCondition.notify_one();


    return oboe::DataCallbackResult::Continue;
}


/**
 * Oboe notifies the application for "about to close the stream".
 *
 * @param oboeStream: the stream to close
 * @param error: oboe's reason for closing the stream
 */
void RecorderOboeEngine::onErrorBeforeClose(oboe::AudioStream *oboeStream,
                                            oboe::Result error) {
    LOGE("%s stream Error before close: %s",
         oboe::convertToText(oboeStream->getDirection()),
         oboe::convertToText(error));
}

/**
 * Oboe notifies application that "the stream is closed"
 *
 * @param oboeStream
 * @param error
 */
void RecorderOboeEngine::onErrorAfterClose(oboe::AudioStream *oboeStream,
                                           oboe::Result error) {
    LOGE("%s stream Error after close: %s",
         oboe::convertToText(oboeStream->getDirection()),
         oboe::convertToText(error));

}

void RecorderOboeEngine::processTasks() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(taskQueueMutex);
            taskCondition.wait(lock, [this] { return !taskQueue.empty() || stopThread; });

            if (stopThread) {
                break;
            }

            task = std::move(taskQueue.front());
            taskQueue.pop();
        }
        task();
    }

}

oboe::Result RecorderOboeEngine::pause() {
    isPause = true;
    return oboe::Result::OK;
}

oboe::Result RecorderOboeEngine::resume() {
    isPause = false;
    return oboe::Result::OK;
}






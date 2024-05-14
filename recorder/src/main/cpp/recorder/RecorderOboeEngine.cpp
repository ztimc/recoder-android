//
// Created by PC on 2024/2/4.
//

#include <memory>
#include <Oscillator.h>
#include <rnnoise-nu.h>

#include "RecorderOboeEngine.h"

RecorderOboeEngine::RecorderOboeEngine() {
    mFileEncoder = std::make_shared<AudioFileEncoder>();
}

oboe::Result RecorderOboeEngine::start(
        int deviceId,
        int channelCount,
        int sampleRate,
        int codec,
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

            LOGD("Stream opened: AudioAPI = %d, channelCount = %d, deviceID = %d",
                 mStream->getAudioApi(),
                 mStream->getChannelCount(),
                 mStream->getDeviceId());

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
        mFileEncoder->close();
    }
    return result;
}

void RecorderOboeEngine::initFile(const char *filePath) {

    int bit = SF_FORMAT_PCM_16;
    if (mAudioFormat == oboe::AudioFormat::I16) {
        bit = SF_FORMAT_PCM_16;
    } else if (mAudioFormat == oboe::AudioFormat::I24) {
        bit = SF_FORMAT_PCM_24;
    } else if (mAudioFormat == oboe::AudioFormat::I32) {
        bit = SF_FORMAT_PCM_32;
    } else if (mAudioFormat == oboe::AudioFormat::Float) { // 这里暂时这样替代
        bit = SF_FORMAT_FLOAT;
    }

    int fileFormat = SF_FORMAT_WAV;
    if (mCodec == WAV) {
        fileFormat = SF_FORMAT_WAV;
    } else if (mCodec == FLAC) {
        fileFormat = SF_FORMAT_FLAC;
    }

    mFileEncoder->initiateWritingToFile(
            filePath,
            mChannelCount,
            mSampleRate,
            fileFormat | bit);
}


oboe::Result RecorderOboeEngine::openStream() {
    oboe::AudioStreamBuilder builder;
    oboe::Result result = builder.setSharingMode(oboe::SharingMode::Exclusive)
            ->setDirection(oboe::Direction::Input)
            ->setInputPreset(oboe::InputPreset::Unprocessed)
            ->setPerformanceMode(oboe::PerformanceMode::None)
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
    int32_t writeLen = mFileEncoder->writeToFile(audioData, numFrames);
    if (writeLen <= 0) {
        LOGE("Error writeToFile. Error: %s", "writeLen <= 0");
        return oboe::DataCallbackResult::Stop;
    }

    float samples[numFrames * oboeStream->getChannelCount()]; // Depending on channel count

    if (mAudioFormat == oboe::AudioFormat::I16) {
        int16_t *data = static_cast<int16_t *>(audioData);

        for (int i = 0; i < numFrames * oboeStream->getChannelCount(); ++i) {
            samples[i] = static_cast<float>(data[i]) / 32767.f;
        }
    } else if (mAudioFormat == oboe::AudioFormat::I24) {
        int32_t *data = static_cast<int32_t *>(audioData);

        for (int i = 0; i < numFrames * oboeStream->getChannelCount(); ++i) {
            samples[i] = static_cast<float>(data[i] >> 8) / 8388607.f;
        }
    } else if (mAudioFormat == oboe::AudioFormat::I32) {
        int32_t *data = static_cast<int32_t *>(audioData);

        for (int i = 0; i < numFrames * oboeStream->getChannelCount(); ++i) {
            samples[i] = static_cast<float>(data[i]) / 2147483647.f;
        }
    } else if (mAudioFormat == oboe::AudioFormat::Float) {
        float *data = static_cast<float *>(audioData);
        for (int i = 0; i < numFrames * oboeStream->getChannelCount(); ++i) {
            samples[i] = data[i];
        }
    }
    LOGI("fucker %s", std::to_string(samples[50]).c_str());

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






//
// Created by PC on 2024/2/4.
//

#include <memory>
#include <Oscillator.h>
#include <rnnoise-nu.h>

#include "RecorderOboeEngine.h"

RecorderOboeEngine::RecorderOboeEngine()
        : mErrorCallback(std::make_shared<DefaultErrorCallback>(*this)) {
}

oboe::Result RecorderOboeEngine::start(
        int deviceId,
        int channelCount,
        int sampleRate,
        oboe::AudioFormat audioFormat) {
    mDeviceId = deviceId;
    mSampleRate = sampleRate;
    mChannelCount = channelCount;
    mAudioFormat = audioFormat;

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
    }
    return result;
}

void RecorderOboeEngine::restart() {
    reset();
    reopenStream();
}

oboe::Result RecorderOboeEngine::reopenStream() {
    if (mStream) {
        stop();
        return start(mDeviceId,
                     mChannelCount,
                     mSampleRate,
                     mAudioFormat);
    } else {
        return oboe::Result::OK;
    }
}

oboe::Result RecorderOboeEngine::openStream() {
    oboe::AudioStreamBuilder builder;
    oboe::Result result = builder.setSharingMode(oboe::SharingMode::Exclusive)
            ->setDirection(oboe::Direction::Input)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setFormat(oboe::AudioFormat::I16)
            ->setFormatConversionAllowed(true)
            ->setSampleRate(mSampleRate)
            ->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Medium)
            ->setDataCallback(this)
            ->setErrorCallback(mErrorCallback)
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
    LOGI("numFrames is %d\n", numFrames);

    return oboe::DataCallbackResult::Continue;
}









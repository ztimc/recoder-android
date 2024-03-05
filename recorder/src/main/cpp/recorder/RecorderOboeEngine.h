//
// Created by zTimC on 2024/2/4.
//

#ifndef WEBRTC_NOISE_ANDROID_RECORDER_OBOE_ENGINE_H
#define WEBRTC_NOISE_ANDROID_RECORDER_OBOE_ENGINE_H

#include <oboe/oboe.h>
#include <DefaultErrorCallback.h>
#include "IRestartable.h"
#include "DefaultDataCallback.h"

class RecorderOboeEngine : public IRestartable, DefaultDataCallback {
public:
    RecorderOboeEngine();

    virtual ~RecorderOboeEngine() = default;


    oboe::Result start(
            int deviceId,
            int channelCount,
            int sampleRate,
            oboe::AudioFormat audioFormat);

    oboe::Result stop();

    // From IRestartable
    void restart() override;

    /*
    * oboe::AudioStreamDataCallback interface implementation
    */
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames) override;


private:
    oboe::Result reopenStream();

    oboe::Result openStream();

    static int constexpr kChannelCount = 2;
    static int constexpr kSampleRate = 48000;

    std::shared_ptr<oboe::AudioStream> mStream;
    std::shared_ptr<DefaultErrorCallback> mErrorCallback;
    std::mutex mLock;

    int32_t mDeviceId = oboe::Unspecified;
    int32_t mChannelCount = oboe::Stereo;
    int32_t mSampleRate = kSampleRate;
    oboe::AudioFormat mAudioFormat = oboe::AudioFormat::I16;


};


#endif //WEBRTC_NOISE_ANDROID_RECORDER_OBOE_ENGINE_H

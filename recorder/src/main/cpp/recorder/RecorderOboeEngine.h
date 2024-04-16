//
// Created by zTimC on 2024/2/4.
//

#ifndef WEBRTC_NOISE_ANDROID_RECORDER_OBOE_ENGINE_H
#define WEBRTC_NOISE_ANDROID_RECORDER_OBOE_ENGINE_H

#include <oboe/oboe.h>
#include <DefaultErrorCallback.h>
#include "IRestartable.h"
#include "DefaultDataCallback.h"
#include "AudioFileEncoder.h"

enum Codec : int32_t {

    /**
     * Use this for wav
     */
    WAV = 1,

    /**
     * Use this for flac.
     */
    FLAC = 2,
};

class RecorderOboeEngine : public oboe::AudioStreamCallback {
public:
    RecorderOboeEngine();

    virtual ~RecorderOboeEngine() = default;


    oboe::Result start(
            int deviceId,
            int channelCount,
            int sampleRate,
            int codec,
            oboe::AudioFormat audioFormat,
            const char *filePath);

    oboe::Result stop();


private:

    oboe::Result openStream();

    static int constexpr kChannelCount = 2;
    static int constexpr kSampleRate = 48000;

    std::shared_ptr<oboe::AudioStream> mStream;
    std::mutex mLock;

    int32_t mDeviceId = oboe::Unspecified;
    int32_t mChannelCount = oboe::Stereo;
    int32_t mSampleRate = kSampleRate;
    int32_t mCodec = WAV;// 1 wav 2 flac
    oboe::AudioFormat mAudioFormat = oboe::AudioFormat::I16;

    std::shared_ptr<AudioFileEncoder> mFileEncoder;

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;
    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error) override;
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) override;

    void initFile(const char *filePath);

};


#endif //WEBRTC_NOISE_ANDROID_RECORDER_OBOE_ENGINE_H

//
// Created by zTimC on 2024/2/4.
//

#ifndef WEBRTC_NOISE_ANDROID_RECORDER_OBOE_ENGINE_H
#define WEBRTC_NOISE_ANDROID_RECORDER_OBOE_ENGINE_H

#include "Oboe.h"
#include "Codec.h"
#include <DefaultErrorCallback.h>
#include "IRestartable.h"
#include "DefaultDataCallback.h"
#include "FormatConverterBox.h"
#include "AudioFileEncoder.h"
#include <jni.h>
#include <thread>
#include <queue>


class RecorderOboeEngine : public oboe::AudioStreamCallback {
public:
    RecorderOboeEngine(JavaVM *javaVM, jobject jObj);

    virtual ~RecorderOboeEngine() = default;


    oboe::Result start(
            int deviceId,
            int channelCount,
            int sampleRate,
            Codec codec,
            oboe::AudioFormat audioFormat,
            const char *filePath);

    oboe::Result stop();

    oboe::Result pause();

    oboe::Result resume();


private:

    oboe::Result openStream();

    static int constexpr kChannelCount = 2;
    static int constexpr kSampleRate = 48000;

    std::shared_ptr<oboe::AudioStream> mStream;
    std::mutex mLock;

    int32_t mDeviceId = oboe::Unspecified;
    int32_t mChannelCount = oboe::Stereo;
    int32_t mSampleRate = kSampleRate;
    Codec mCodec = WAV;// 1 wav 2 flac
    oboe::AudioFormat mAudioFormat = oboe::AudioFormat::I16;
    bool isPause = false;

    std::shared_ptr<AudioFileEncoder> mFileEncoder;
    std::unique_ptr<FormatConverterBox> mInputConverter;

    std::thread taskThread;
    std::queue<std::function<void()>> taskQueue;
    std::condition_variable taskCondition;
    std::mutex taskQueueMutex;
    bool stopThread = false;

    void processTasks();

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;
    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error) override;
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) override;

    void initFile(const char *filePath);

    JavaVM *mJavaVM;
    jobject mJObj;

};


#endif //WEBRTC_NOISE_ANDROID_RECORDER_OBOE_ENGINE_H

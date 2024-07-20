//
// Created by Sabinetek on 2024/6/13.
//

#ifndef WEBRTC_NOISE_ANDROID_AACENCODER_H
#define WEBRTC_NOISE_ANDROID_AACENCODER_H

#include "AudioCodec.h"
#include "Definitions.h"
#include "aacenc_lib.h"
#include "FifoBuffer.h"
#include <cstdio>
#include <queue>

class AACEncoder : public AudioCodec  {
public:

    int32_t createCodec(const char *outFileName, int32_t outputChannels, int32_t sampleRate, oboe::AudioFormat format) override;

    int32_t write(void *audioData, int32_t samples) override;

    void destroyCodec() override;


private:
    HANDLE_AACENCODER mHandle;
    AACENC_InfoStruct mInfo = { 0 };
    int32_t mChannel;
    oboe::AudioFormat mFormat;
    FILE *mAACFile = nullptr;
    std::unique_ptr<oboe::FifoBuffer>   mFifoBuffer{};
};

#endif //WEBRTC_NOISE_ANDROID_AACENCODER_H

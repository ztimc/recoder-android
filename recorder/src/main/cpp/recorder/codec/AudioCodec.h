//
// Created by Sabinetek on 2024/6/12.
//

#ifndef WEBRTC_NOISE_ANDROID_AUDIOCODEC_H
#define WEBRTC_NOISE_ANDROID_AUDIOCODEC_H


#include <stdint.h>
#include "oboe/Definitions.h"
#include "../Codec.h"

class AudioCodec {
    virtual int32_t createCodec(const char *outFileName,
                                int32_t outputChannels,
                                int32_t sampleRate,
                                oboe::AudioFormat format) = 0;

    virtual int32_t write(void *audioData, int32_t samples) = 0;

    virtual void destroyCodec() = 0;
};


#endif //WEBRTC_NOISE_ANDROID_AUDIOCODEC_H

//
// Created by Sabinetek on 2024/5/23.
//

#ifndef WEBRTC_NOISE_ANDROID_AUDIOENCODER_H
#define WEBRTC_NOISE_ANDROID_AUDIOENCODER_H

#include <stdint.h>

class AudioEncoder {
public:
    virtual ~AudioEncoder() = default;

    virtual int32_t initCodec(int /*sampleRate*/, int /*channel*/, int /*bitDepth*/) { return 0; }

    virtual int32_t encode(void * /*pcm*/, int32_t /*samples*/) { return 0; }

    virtual int32_t deInitCodec() { return 0; }

};


#endif //WEBRTC_NOISE_ANDROID_AUDIOENCODER_H

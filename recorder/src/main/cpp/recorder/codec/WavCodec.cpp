//
// Created by Sabinetek on 2024/6/12.
//

#include "WavCodec.h"

int32_t WavCodec::createCodec(const char *outFileName,
                              int32_t outputChannels,
                              int32_t sampleRate,
                              oboe::AudioFormat format) {


    return 0;
}

int32_t WavCodec::write(void *audioData, int32_t samples) {
    tinywav_write_f(mTinyWav, audioData, samples);
}

void WavCodec::destroyCodec() {

}

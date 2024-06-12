//
// Created by Sabinetek on 2024/6/12.
//

#include "WavCodec.h"

int32_t WavCodec::createCodec(const char *outFileName,
                              int32_t outputChannels,
                              int32_t sampleRate,
                              oboe::AudioFormat format) {

    int bit = SF_FORMAT_PCM_16;
    if (format == oboe::AudioFormat::I16) {
        bit = SF_FORMAT_PCM_16;
    } else if (format == oboe::AudioFormat::I24) {
        bit = SF_FORMAT_PCM_24;
    } else if (format == oboe::AudioFormat::I32) {
        bit = SF_FORMAT_PCM_32;
    } else if (format == oboe::AudioFormat::Float) { // 这里暂时这样替代
        bit = SF_FORMAT_FLOAT;
    }

    mHandler = new SndfileHandle(outFileName,
                                 SFM_WRITE,
                                 SF_FORMAT_WAV | bit,
                                 outputChannels,
                                 sampleRate);


    return 0;
}

int32_t WavCodec::write(void *audioData, int32_t samples) {

    int subMask = ((mHandler->format()) & SF_FORMAT_SUBMASK);

    if (subMask == SF_FORMAT_PCM_16) {
        return mHandler->write((short *) audioData, samples * mHandler->channels());
    } else if (subMask == SF_FORMAT_PCM_24) {
        return mHandler->writeRaw(audioData, 3 * samples * mHandler->channels());
    } else if (subMask == SF_FORMAT_PCM_32) {
        return mHandler->write((int *) audioData, samples * mHandler->channels());
    } else if (subMask == SF_FORMAT_FLOAT) {
        return mHandler->write((float *) audioData, samples * mHandler->channels());
    }
    return -1;
}

void WavCodec::destroyCodec() {
    delete mHandler;
}

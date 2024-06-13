//
// Created by Sabinetek on 2024/6/12.
//

#ifndef WEBRTC_NOISE_ANDROID_WAVCODEC_H
#define WEBRTC_NOISE_ANDROID_WAVCODEC_H

#include "AudioCodec.h"
#include "sndfile.hh"


class WavCodec : public AudioCodec {

public:
    int32_t createCodec(const char *outFileName, int32_t outputChannels, int32_t sampleRate,
                        oboe::AudioFormat format) override ;

    int32_t write(void *audioData, int32_t samples) override;

    void destroyCodec() override;
private:
    SndfileHandle *mHandler = nullptr;
};


#endif //WEBRTC_NOISE_ANDROID_WAVCODEC_H

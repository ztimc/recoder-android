//
// Created by Sabinetek on 2024/4/16.
//

#ifndef WEBRTC_NOISE_ANDROID_AUDIOFILEENCODER_H
#define WEBRTC_NOISE_ANDROID_AUDIOFILEENCODER_H

#include <stdint.h>
#include "sndfile.hh"

class AudioFileEncoder {

public:

    void initiateWritingToFile(const char *outFileName,
                               int32_t outputChannels,
                               int32_t sampleRate,
                               int32_t format);

    int32_t writeToFile(void *audioData, int32_t numFrames);

private:

    const char* TAG = "AudioFileEncoder:: %s";

    SndfileHandle *mHandler = nullptr;

};


#endif //WEBRTC_NOISE_ANDROID_AUDIOFILEENCODER_H

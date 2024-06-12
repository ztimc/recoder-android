//
// Created by Sabinetek on 2024/4/16.
//

#include "AudioFileEncoder.h"
#include "logging_macros.h"


void AudioFileEncoder::initiateWritingToFile(const char *outFileName,
                                             int32_t outputChannels,
                                             int32_t sampleRate,
                                             oboe::AudioFormat format,
                                             Codec codec) {

}

int32_t AudioFileEncoder::writeToFile(void *audioData, int32_t numFrames) {
    return 0;
}

void AudioFileEncoder::close() {

}





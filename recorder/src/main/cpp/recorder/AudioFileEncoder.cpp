//
// Created by Sabinetek on 2024/4/16.
//

#include "AudioFileEncoder.h"
#include "logging_macros.h"
#include "WavCodec.h"


void AudioFileEncoder::openAudioFile(const char *outFileName,
                                     int32_t outputChannels,
                                     int32_t sampleRate,
                                     oboe::AudioFormat format,
                                     Codec codec) {
    switch (codec) {
        case WAV:
            mFileEncoder = std::make_shared<WavCodec>();
            mFileEncoder->createCodec(outFileName, outputChannels, sampleRate, format);
            break;
        case FLAC:
            break;
    }
}

int32_t AudioFileEncoder::writeToFile(void *audioData, int32_t numFrames) {
   return mFileEncoder->write(audioData, numFrames);
}

void AudioFileEncoder::closeFile() {
    mFileEncoder->destroyCodec();
}





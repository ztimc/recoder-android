//
// Created by Sabinetek on 2024/4/16.
//

#include "AudioFileEncoder.h"
#include "logging_macros.h"
#include "WavCodec.h"
#include "FlacEncoder.h"
#include "AACEncoder.h"

void AudioFileEncoder::openAudioFile(const char *outFileName,
                                     int32_t outputChannels,
                                     int32_t sampleRate,
                                     oboe::AudioFormat format,
                                     RecorderCodec codec) {
    switch (codec) {
        case RECORDER_WAV:
            mFileEncoder = std::make_shared<WavCodec>();
            mFileEncoder->createCodec(outFileName, outputChannels, sampleRate, format);
            break;
        case RECORDER_FLAC:
            mFileEncoder = std::make_shared<FlacEncoder>();
            mFileEncoder->createCodec(outFileName, outputChannels, sampleRate, format);
            break;
        case RECORDER_AAC:
            mFileEncoder = std::make_shared<AACEncoder>();
            mFileEncoder->createCodec(outFileName, outputChannels, sampleRate, format);
            break;
    }
}

int32_t AudioFileEncoder::writeToFile(void *audioData, int32_t numFrames) {
    return mFileEncoder->write(audioData, numFrames);
}

void AudioFileEncoder::closeFile() {
    mFileEncoder->destroyCodec();
}





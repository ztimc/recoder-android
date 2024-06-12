//
// Created by Sabinetek on 2024/4/16.
//

#ifndef WEBRTC_NOISE_ANDROID_AUDIOFILEENCODER_H
#define WEBRTC_NOISE_ANDROID_AUDIOFILEENCODER_H

#include <stdint.h>
#include "Codec.h"
#include "Oboe.h"
#include "AudioCodec.h"

class AudioFileEncoder {

public:

    void openAudioFile(const char *outFileName,
                       int32_t outputChannels,
                       int32_t sampleRate,
                       oboe::AudioFormat format,
                       Codec codec);

    int32_t writeToFile(void *audioData, int32_t numFrames);

    void closeFile();

private:

    const char* TAG = "AudioFileEncoder:: %s";

    std::shared_ptr<AudioCodec> mFileEncoder;
};


#endif //WEBRTC_NOISE_ANDROID_AUDIOFILEENCODER_H

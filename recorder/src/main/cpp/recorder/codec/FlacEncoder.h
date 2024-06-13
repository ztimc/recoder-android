//
// Created by Sabinetek on 2024/6/13.
//

#ifndef WEBRTC_NOISE_ANDROID_FLACENCODER_H
#define WEBRTC_NOISE_ANDROID_FLACENCODER_H

#include "FLAC++/metadata.h"
#include "FLAC++/encoder.h"
#include "AudioCodec.h"

class FlacEncoder : public FLAC::Encoder::File, public AudioCodec {
public:
    FlacEncoder() : FLAC::Encoder::File() {}

    int32_t createCodec(const char *outFileName, int32_t outputChannels, int32_t sampleRate,
                        oboe::AudioFormat format) override;

    int32_t write(void *audioData, int32_t samples) override;

    void destroyCodec() override;

protected:
    void progress_callback(FLAC__uint64 bytes_written, FLAC__uint64 samples_written,
                           uint32_t frames_written, uint32_t total_frames_estimate) override;

private:
    FLAC__StreamMetadata *metadata[2];
    FLAC__StreamMetadata_VorbisComment_Entry entry;
    int bps = 0;
    int channel = 0;
};


#endif //WEBRTC_NOISE_ANDROID_FLACENCODER_H

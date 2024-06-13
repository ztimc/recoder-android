//
// Created by Sabinetek on 2024/6/13.
//

#include "FlacEncoder.h"

int32_t
FlacEncoder::createCodec(const char *outFileName, int32_t outputChannels, int32_t sampleRate,
                         oboe::AudioFormat format) {
    bool ok = true;
    FLAC__StreamEncoderInitStatus init_status;
    channel = outputChannels;

    if (format == oboe::AudioFormat::I16) {
        bps = 16;
    } else if (format == oboe::AudioFormat::I24) {
        bps = 24;
    } else {
        return -1;
    }

    ok &= set_verify(true);
    ok &= set_compression_level(5);
    ok &= set_channels(outputChannels);
    ok &= set_bits_per_sample(bps);
    ok &= set_sample_rate(sampleRate);
    if (ok) {
        if (
                (metadata[0] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT)) ==
                NULL ||
                (metadata[1] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING)) == NULL ||
                /* there are many tag (vorbiscomment) functions but these are convenient for this particular use: */
                !FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "ARTIST",
                                                                                "Some Artist") ||
                !FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/
                                                                    false) ||
                /* copy=false: let metadata object take control of entry's allocated string */
                !FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "YEAR",
                                                                                "1984") ||
                !FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/
                                                                    false)
                ) {
            fprintf(stderr, "ERROR: out of memory or tag error\n");
            ok = false;
        } else {
            metadata[1]->length = 1234; /* set the padding length */
            ok = set_metadata(metadata, 2);
        }
    }

    if (ok) {
        init_status = init(outFileName);
        if (init_status != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
            fprintf(stderr, "ERROR: initializing encoder: %s\n",
                    FLAC__StreamEncoderInitStatusString[init_status]);
            ok = false;
        }
    }
    return ok ? 0 : -1;
}

void pcm16_to_flac_int32(const int16_t *pcm16, FLAC__int32 *flac_int32, size_t num_samples,
                         int num_channels) {
    for (size_t i = 0; i < num_samples * num_channels; ++i) {
        flac_int32[i] = static_cast<FLAC__int32>(pcm16[i]);
    }
}

void pcm24_to_flac_int32(const uint8_t *pcm24, FLAC__int32 *flac_int32, size_t num_samples,
                         int num_channels) {
    for (size_t i = 0; i < num_samples * num_channels; ++i) {
        FLAC__int32 sample = (pcm24[3 * i] << 16) | (pcm24[3 * i + 1] << 8) | pcm24[3 * i + 2];
        if (sample & 0x00800000) // if the sign bit is set
            sample |= 0xFF000000; // sign extend to 32 bits

        flac_int32[i] = sample;
    }
}

int32_t FlacEncoder::write(void *audioData, int32_t samples) {
    FLAC__int32 pcm[samples * channel];
    if (bps == 16) {
        pcm16_to_flac_int32((short *) audioData,
                            pcm,
                            samples,
                            channel);
    } else if (bps == 24) {
        pcm24_to_flac_int32((uint8_t *) audioData,
                            pcm,
                            samples,
                            channel);
    } else {
        return -1;
    }

    if (process_interleaved(pcm, samples)) {
        return samples;
    }

    return 0;
}

void FlacEncoder::destroyCodec() {

    finish();
    FLAC__metadata_object_delete(metadata[0]);
    FLAC__metadata_object_delete(metadata[1]);

}


void FlacEncoder::progress_callback(FLAC__uint64 bytes_written, FLAC__uint64 samples_written,
                                    uint32_t frames_written, uint32_t total_frames_estimate) {

}


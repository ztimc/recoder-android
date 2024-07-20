//
// Created by Sabinetek on 2024/6/13.
//

#include "AACEncoder.h"
#include "logging_macros.h"
#include "Utilities.h"

int32_t AACEncoder::createCodec(const char *outFileName, int32_t outputChannels, int32_t sampleRate,
                                oboe::AudioFormat format) {
    this->mChannel = outputChannels;
    this->mFormat = format;
    this->mAACFile = fopen(outFileName, "wb");
    if (this->mAACFile == nullptr) {
        return -1;
    }

    if (aacEncOpen(&mHandle, 0, outputChannels) != AACENC_OK) {
        return -1;
    }
    if (aacEncoder_SetParam(mHandle, AACENC_AOT, 2) != AACENC_OK) {
        return -2;
    }
    if (aacEncoder_SetParam(mHandle,
                            AACENC_SAMPLERATE,
                            sampleRate) != AACENC_OK) {
        return -3;
    }
    if (aacEncoder_SetParam(mHandle,
                            AACENC_CHANNELMODE,
                            outputChannels == 1 ? MODE_1 : MODE_2) != AACENC_OK) {
        return -4;
    }
    if (aacEncoder_SetParam(mHandle, AACENC_CHANNELORDER, 1) != AACENC_OK) {
        return -5;
    }
    if (aacEncoder_SetParam(mHandle, AACENC_BITRATEMODE, 5) != AACENC_OK) {
        return -6;
    }
    if (aacEncoder_SetParam(mHandle, AACENC_TRANSMUX, TT_MP4_ADTS) != AACENC_OK) {
        return -7;
    }
    if (aacEncoder_SetParam(mHandle, AACENC_AFTERBURNER, 1) != AACENC_OK) {
        return -8;
    }
    if (aacEncEncode(mHandle,
                     nullptr,
                     nullptr,
                     nullptr,
                     nullptr) != AACENC_OK) {
        return -9;
    }
    if (aacEncInfo(mHandle, &mInfo) != AACENC_OK) {
        return -10;
    }
    this->mFifoBuffer = std::make_unique<oboe::FifoBuffer>(2 * mChannel, 1024 * 10);
    return 0;
}

int32_t AACEncoder::write(void *audioData, int32_t samples) {

    this->mFifoBuffer->write(audioData, samples);

    if (this->mFifoBuffer->getFullFramesAvailable() < mInfo.frameLength) {
        return 0;
    }
    AACENC_BufDesc in_buf = {0}, out_buf = {0};
    AACENC_InArgs in_args = {0};
    AACENC_OutArgs out_args = {0};
    int in_identifier = IN_AUDIO_DATA;
    int in_size, in_elem_size;
    int out_identifier = OUT_BITSTREAM_DATA;
    int out_size, out_elem_size;
    void *in_ptr, *out_ptr;
    uint8_t outbuf[2048];
    uint8_t inbuf[1024 * 10];
    AACENC_ERROR err;

    this->mFifoBuffer->read(inbuf, (int32_t) mInfo.frameLength);

    in_ptr = inbuf;
    in_size = (int)mInfo.frameLength * 2 * mChannel;
    in_elem_size = 2;

    in_args.numInSamples = (int)mInfo.frameLength;
    in_buf.numBufs = 1;
    in_buf.bufs = &in_ptr;
    in_buf.bufferIdentifiers = &in_identifier;
    in_buf.bufSizes = &in_size;
    in_buf.bufElSizes = &in_elem_size;

    out_ptr = outbuf;
    out_size = sizeof(outbuf);
    out_elem_size = 1;
    out_buf.numBufs = 1;
    out_buf.bufs = &out_ptr;
    out_buf.bufferIdentifiers = &out_identifier;
    out_buf.bufSizes = &out_size;
    out_buf.bufElSizes = &out_elem_size;

    if ((err = aacEncEncode(mHandle, &in_buf, &out_buf, &in_args, &out_args)) != AACENC_OK) {
        return -1;
    }
    if (out_args.numOutBytes <= 0)
        return -1;

    fwrite(outbuf, 1, out_args.numOutBytes, mAACFile);
    return out_args.numOutBytes;
}

void AACEncoder::destroyCodec() {
    fclose(mAACFile);
    aacEncClose(&mHandle);
}


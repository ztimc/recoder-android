//
// Created by Sabinetek on 2024/4/16.
//

#include "AudioFileEncoder.h"
#include "logging_macros.h"

void AudioFileEncoder::initiateWritingToFile(const char *outFileName, int32_t outputChannels,
                                             int32_t sampleRate, int32_t format) {
    if (mHandler != nullptr) {
        delete mHandler;
    }

    mHandler = new SndfileHandle(outFileName,
                                 SFM_WRITE,
                                 format,
                                 outputChannels,
                                 sampleRate);

    LOGD (TAG, "readFileInfo(): Opened file => ");
    LOGD(TAG, outFileName);

    LOGI(TAG, "readFileInfo(): Sample rate :");
    LOGI(TAG, std::to_string(mHandler->samplerate()).c_str());
    LOGI(TAG, "readFileInfo(): Channels :");
    LOGI(TAG, std::to_string(mHandler->channels()).c_str());
    LOGI(TAG, "readFileInfo(): Frames : ");
    LOGI(TAG, std::to_string(mHandler->frames()).c_str());
}

int32_t AudioFileEncoder::writeToFile(void *audioData, int32_t numFrames) {
    if (mHandler == NULL) {
        LOGE(TAG, "Please call initiateWritingToFile");
        return 0;
    }

    int subMask = ((mHandler->format()) & SF_FORMAT_SUBMASK);

    if (subMask == SF_FORMAT_PCM_16) {
        return mHandler->write((short *) audioData, numFrames * mHandler->channels());
    } else if (subMask == SF_FORMAT_PCM_24) {
        return mHandler->writeRaw(audioData, 3 * numFrames * mHandler->channels());
    } else if (subMask == SF_FORMAT_PCM_32) {
        return mHandler->write((int *) audioData, numFrames * mHandler->channels());
    } else if (subMask == SF_FORMAT_FLOAT) {
        return mHandler->write((float *) audioData, numFrames * mHandler->channels());
    }
    LOGE(TAG, "Not support format");
    return 0;
}

void AudioFileEncoder::close() {
    if (mHandler != nullptr) {
        delete mHandler;
    }
}



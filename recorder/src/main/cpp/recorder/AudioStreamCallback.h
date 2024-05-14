//
// Created by Sabinetek on 2024/5/14.
//

#ifndef WEBRTC_NOISE_ANDROID_AUDIOSTREAMCALLBACK_H
#define WEBRTC_NOISE_ANDROID_AUDIOSTREAMCALLBACK_H

#include <jni.h>
#include <oboe/Oboe.h>
#include <vector>
#include <mutex>
#include <android/log.h>

class AudioStreamCallback : public oboe::AudioStreamCallback {
public:
    AudioStreamCallback(JNIEnv *env, jobject audioEngine) {
        mEnv = env;
        mAudioEngine = env->NewGlobalRef(audioEngine);
        jclass clazz = env->GetObjectClass(audioEngine);
        // Obtain the Java method ID to use
        mMethodID = env->GetMethodID(clazz, "onAudioDataRecorded", "([B)V");
    }

    ~AudioStreamCallback() {
        mEnv->DeleteGlobalRef(mAudioEngine);
    }

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override {
        oboe::AudioFormat format = oboeStream->getFormat();

        // convert to float 0 - -1
        if (format == oboe::AudioFormat::I16) {
        } else if (format == oboe::AudioFormat::I24) {
        } else if (format == oboe::AudioFormat::I32) {
        } else if (format == oboe::AudioFormat::Float) { // 这里暂时这样替代
        }
        jbyteArray byteArray = mEnv->NewByteArray(numFrames * sizeof(float));
        mEnv->SetByteArrayRegion(byteArray, 0, numFrames * sizeof(float),
                                 (const jbyte *) audioData);

        // Call the Java method
        mEnv->CallVoidMethod(mAudioEngine, mMethodID, byteArray);

        // Release the Java array
        mEnv->DeleteLocalRef(byteArray);

        return oboe::DataCallbackResult::Continue;
    }

private:
    JNIEnv *mEnv;
    jobject mAudioEngine;
    jmethodID mMethodID;
};

#endif //WEBRTC_NOISE_ANDROID_AUDIOSTREAMCALLBACK_H

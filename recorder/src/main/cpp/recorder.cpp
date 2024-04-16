#include <jni.h>
#include <string>

#include "recorder/RecorderOboeEngine.h"

static RecorderOboeEngine *engine;


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_sabinetek_recorder_AudioRecorder_create(JNIEnv *env, jobject thiz) {
    engine = new RecorderOboeEngine();
    return (engine != nullptr);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sabinetek_recorder_AudioRecorder_delete(JNIEnv *env, jobject thiz) {
    delete engine;
    engine = nullptr;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_sabinetek_recorder_AudioRecorder_startRecording(JNIEnv *env, jobject thiz, jint device_id,
                                                         jstring file_path, jint channel,
                                                         jint sample_rate, jint codec,
                                                         jint bit_depth) {
    const char *path = env->GetStringUTFChars(file_path, nullptr);

    oboe::AudioFormat format = oboe::AudioFormat::I16;
    if (bit_depth == 16) {
        format = oboe::AudioFormat::I16;
    } else if (bit_depth == 24) {
        format = oboe::AudioFormat::I24;
    } else if (bit_depth == 32) {
        format = oboe::AudioFormat::I32;
    } else if (bit_depth == 64) {
        format = oboe::AudioFormat::I24;
    }

    engine->start(oboe::Unspecified,
                  channel,
                  sample_rate,
                  codec,
                  format,
                  path);

    env->ReleaseStringUTFChars(file_path, path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sabinetek_recorder_AudioRecorder_stopRecording(JNIEnv *env, jobject thiz) {
    engine->stop();
}
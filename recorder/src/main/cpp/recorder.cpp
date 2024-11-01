#include <jni.h>
#include <string>

#include "recorder/RecorderOboeEngine.h"
#include "recorder/RecorderCodec.h"

static RecorderOboeEngine *engine;


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_sabinetek_recorder_AudioRecorder_create(JNIEnv *env, jobject thiz) {
    JavaVM *javaVM;
    env->GetJavaVM(&javaVM);
    jobject globalRef = env->NewGlobalRef(thiz);
    engine = new RecorderOboeEngine(javaVM, globalRef);
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
        format = oboe::AudioFormat::Float;
    }

    engine->start(device_id,
                  channel,
                  sample_rate,
        static_cast<RecorderCodec>(codec),
                  format,
                  path);

    env->ReleaseStringUTFChars(file_path, path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sabinetek_recorder_AudioRecorder_stopRecording(JNIEnv *env, jobject thiz) {
    engine->stop();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sabinetek_recorder_AudioRecorder_pause(JNIEnv *env, jobject thiz) {
     engine->pause();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sabinetek_recorder_AudioRecorder_resume(JNIEnv *env, jobject thiz) {
    engine->resume();
}
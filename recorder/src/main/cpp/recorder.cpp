#include <jni.h>
#include <string>

#include "recorder/RecorderOboeEngine.h"

static RecorderOboeEngine *engine;

extern "C" JNIEXPORT jstring JNICALL
Java_com_sabinetek_recorder_NativeLib_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void  JNICALL
Java_com_sabinetek_recorder_NativeLib_startRecorder(JNIEnv *env, jobject thiz) {
    engine = new RecorderOboeEngine();
    engine->start(oboe::Unspecified,
                  2,
                  48000,
                  oboe::AudioFormat::Float);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_sabinetek_recorder_NativeLib_stopRecorder(JNIEnv *env, jobject thiz) {
    engine->stop();
}
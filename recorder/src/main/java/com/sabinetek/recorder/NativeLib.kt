package com.sabinetek.recorder

class NativeLib {

    /**
     * A native method that is implemented by the 'recorder' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    external fun startRecorder()

    external fun stopRecorder()

    companion object {
        // Used to load the 'recorder' library on application startup.
        init {
            System.loadLibrary("recorder")
        }
    }
}
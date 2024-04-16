package com.sabinetek.recorder

class AudioRecorder {

    /**
     * A native method that is implemented by the 'recorder' native library,
     * which is packaged with this application.
     */

    // Native methods
    external fun create(): Boolean
    external fun delete()

    external fun startRecording(
        deviceId: Int,
        filePath: String,
        channel: Int,
        sampleRate: Int,
        codec: Int,
        bitDepth: Int
    )

    external fun stopRecording()



    companion object {
        // Used to load the 'recorder' library on application startup.
        init {
            System.loadLibrary("recorder")
        }
    }
}
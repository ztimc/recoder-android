package com.sabinetek.recorder

class AudioRecorder {

    /**
     * A native method that is implemented by the 'recorder' native library,
     * which is packaged with this application.
     */
    interface Callback {
        fun onAudioReady(data: FloatArray)
    }

    var callback: AudioRecorder.Callback? = null

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


    //Callback
    fun onAudioData(data: FloatArray) {
        callback?.onAudioReady(data)
    }

    init {
        System.loadLibrary("recorder")
    }


}
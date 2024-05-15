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

    // 回调函数
    fun onAudioData(data: FloatArray) {
        // 处理录音数据
        println("Received audio data: ${data.joinToString()}")
    }

    init {
        System.loadLibrary("recorder")
    }

}
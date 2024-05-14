package com.sabinetek.noise

import android.content.Context
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.ui.Modifier
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.sabinetek.noise.ui.theme.WebrtcnoiseandroidTheme
import com.sabinetek.recorder.AudioRecorder

class MainActivity : ComponentActivity() {

    private val audioRecorder = AudioRecorder()
    private val requestPermissionLauncher =
        registerForActivityResult(
            ActivityResultContracts.RequestPermission()
        ) { isGranted: Boolean ->
            if (isGranted) {
                Log.i("Permission: ", "Granted")
            } else {
                Log.i("Permission: ", "Denied")
            }
        }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            WebrtcnoiseandroidTheme {
                // A surface container using the 'background' color from the theme
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    Column {
                        Button(onClick = {
                            Thread {
                                audioRecorder.create()
                                audioRecorder.startRecording(
                                    0,
                                    baseContext.filesDir.absolutePath + "/${System.currentTimeMillis()}.wav",
                                    2,
                                    48000,
                                    1,
                                    32
                                )
                            }.start()
                        }) {
                            Text(text = "start")
                        }
                        Button(onClick = {
                            audioRecorder.stopRecording()
                            audioRecorder.delete()
                        }) {
                            Text(text = "stop")
                        }
                    }
                }
            }


        }
        onClickRequestPermission(this)
    }


    private fun onClickRequestPermission(context: Context) {
        when {
            ContextCompat.checkSelfPermission(
                this,
                android.Manifest.permission.RECORD_AUDIO
            ) == PackageManager.PERMISSION_GRANTED -> {
                Toast.makeText(context, "牛逼", Toast.LENGTH_LONG).show()
            }

            ActivityCompat.shouldShowRequestPermissionRationale(
                this,
                android.Manifest.permission.RECORD_AUDIO
            ) -> {
                requestPermissionLauncher.launch(
                    android.Manifest.permission.RECORD_AUDIO
                )
            }

            else -> {
                requestPermissionLauncher.launch(
                    android.Manifest.permission.RECORD_AUDIO
                )
            }
        }
    }
}
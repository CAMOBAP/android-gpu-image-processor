package com.camobap.imgmixer

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import java.io.File
import android.content.Intent
import android.net.Uri
import androidx.core.content.FileProvider




class MainActivity : AppCompatActivity() {

    val imageMixer: ImageMixer = ImageMixer(1024, 1024)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        AssetsExtractor.extractDir(this, "", filesDir, true)

        for (i in 1..4) {
            imageMixer.load(File(filesDir, "$i.jpg"))
        }
    }

    fun doProcessing(view: View) {
        val resultFile = File(filesDir, "result.bmp")

        imageMixer.convert(resultFile);

        val intent = Intent()
        intent.action = Intent.ACTION_VIEW
        val uri = FileProvider.getUriForFile(this, BuildConfig.APPLICATION_ID + ".provider", resultFile.absoluteFile)
        intent.setDataAndType(uri, "image/*")
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        startActivity(intent)
    }
}

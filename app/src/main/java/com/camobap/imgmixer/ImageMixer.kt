package com.camobap.imgmixer

import androidx.annotation.Keep

import java.io.File

@Keep
class ImageMixer(width: Int, height: Int) {

    @Keep
    private var nativeHandle: Long = 0

    init {
        initialize(width, height)
    }

    fun load(imagePath: File) {
        load0(imagePath.absolutePath)
    }

    fun convert(outFile: File) {
        convert0(outFile.absolutePath)
    }

    @Keep
    external fun clear()

    @Keep
    private external fun convert0(outPath: String)

    @Keep
    private external fun load0(imagePath: String)

    @Keep
    protected external fun initialize(width: Int, height: Int)

    @Keep
    @Throws(Throwable::class)
    protected external fun finalize()

    companion object {
        init {
            System.loadLibrary("ndk-shader-mixer")
        }
    }
}

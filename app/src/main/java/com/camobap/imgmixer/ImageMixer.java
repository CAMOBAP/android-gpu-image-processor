package com.camobap.imgmixer;

import androidx.annotation.Keep;

import java.io.File;

@Keep
public final class ImageMixer {
    static {
        System.loadLibrary("ndk-shader-mixer");
    }

    public ImageMixer() {
        initialize();
    }

    public File convert(File image0, File image1, File image2, File image3) {
        return new File(convert0(image0.getAbsolutePath(),
                image1.getAbsolutePath(),
                image2.getAbsolutePath(),
                image3.getAbsolutePath()));
    }

    @Keep
    private native String convert0(String image0, String image1, String image2, String image3);

    @Keep
    protected native void initialize();

    @Keep
    protected native void finalize() throws Throwable;
}

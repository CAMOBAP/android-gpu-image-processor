package com.camobap.imgmixer;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;

import java.io.File;

@Keep
public final class ImageMixer {
    static {
        System.loadLibrary("ndk-shader-mixer");
    }

    public ImageMixer(int width, int height) {
        initialize(width, height);
    }

    public void load(@NonNull File imagePath) {
        load0(imagePath.getAbsolutePath());
    }

    public void convert(@NonNull File outFile) {
        convert0(outFile.getAbsolutePath());
    }

    @Keep
    public native void clear();

    @Keep
    private native void convert0(@NonNull String outPath);

    @Keep
    private native void load0(@NonNull String imagePath);

    @Keep
    protected native void initialize(int width, int height);

    @Keep
    protected native void finalize() throws Throwable;

    @Keep
    private long nativeHandle;
}

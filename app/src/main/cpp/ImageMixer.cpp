//
// Created by CAMOBAP on 2019-05-03.
//

#include "ImageMixer.hpp"

namespace imgmixer {

ImageMixer::ImageMixer(jni::JNIEnv &env)
{}

ImageMixer::~ImageMixer()
{}

jni::Local<jni::String> ImageMixer::convert(jni::JNIEnv &env,
                                            jni::String& image0,
                                            jni::String& image1,
                                            jni::String& image2,
                                            jni::String& image3)
{
    // TODO all opengl stuff here

    return jni::Local<jni::String>();
}

};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    jni::JNIEnv& env { jni::GetEnv(*vm) };

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    using imgmixer::ImageMixer;

    jni::RegisterNativePeer<ImageMixer>(env,
            jni::Class<ImageMixer>::Find(env),
            "nativeHandle",
            jni::MakePeer<ImageMixer>,
            "initialize",
            "finalize",
            METHOD(&ImageMixer::convert, "convert0")
    );

#undef METHOD

    return jni::Unwrap(jni::jni_version_1_2);
}

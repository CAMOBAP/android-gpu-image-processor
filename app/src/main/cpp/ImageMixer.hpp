//
// Created by CAMOBAP on 2019-05-03.
//

#pragma once

#include <jni/jni.hpp>

namespace imgmixer {

class ImageMixer final {
public:
    static constexpr auto Name() { return "com/camobap/imgmixer/ImageMixer"; }

    explicit ImageMixer(jni::JNIEnv &);
    ~ImageMixer();
    ImageMixer(const ImageMixer &) = delete;

    jni::Local<jni::String> convert(jni::JNIEnv &env,
                                    jni::String& image0,
                                    jni::String& image1,
                                    jni::String& image2,
                                    jni::String& image3);
};

};

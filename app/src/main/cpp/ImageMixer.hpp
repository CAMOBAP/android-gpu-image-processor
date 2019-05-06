//
// Created by CAMOBAP on 2019-05-03.
//

#pragma once

#include <jni/jni.hpp>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <vector>

namespace imgmixer {

class ImageMixer final {
public:
    static constexpr auto Name() { return "com/camobap/imgmixer/ImageMixer"; }

    explicit ImageMixer(jni::JNIEnv &, jni::jint width, jni::jint height);
    ~ImageMixer();
    ImageMixer(const ImageMixer &) = delete;

    void loadTexture(jni::JNIEnv &env, jni::String &imagePath);

    void clearTextures(jni::JNIEnv &env);

    void convert(jni::JNIEnv &env, jni::String &outPath);

private:
    EGLDisplay eglDisplay = EGL_NO_DISPLAY;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    EGLContext eglContext = EGL_NO_CONTEXT;

    GLuint glProgram = GL_NONE;
    GLint gvPositionHandle = GL_NONE;

    GLuint quadBuffer = GL_NONE;
//    GLfloat quadVertices[2*4] { -0.5f, -0.5f,
//                                +0.5f, -0.5f,
//                                -0.5f, +0.5f,
//                                +0.5f, +0.5f };
    GLfloat quadVertices[2*4] { -1, -1,
                                +1, -1,
                                -1, +1,
                                +1, +1 };

    std::vector<GLuint> textures = {};
};

};

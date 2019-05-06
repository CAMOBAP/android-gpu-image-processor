//
// Created by CAMOBAP on 2019-05-03.
//

#include "ImageMixer.hpp"

#include <SOIL.h>

#include "IllegalStateException.hpp"
#define ANDROID_LOG_TAG "ImageMixer++"
#include "stddefs.h"

#include "oglshaderutils.hpp"

namespace imgmixer {

static constexpr std::size_t MAX_IMAGES = 4;
static thread_local bool is_egl_context_creator_thread = false;

static const char gVertexShader[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 aPosition;\n"
            "out vec2 vTexCoord;\n"
            "void main() {\n"
            "  gl_Position = vec4(aPosition, 0.0, 1.0);\n"
            "  vTexCoord = (aPosition + 1.0) / 2.0;\n"
            "}\n";

static const char gFragmentShader[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "uniform sampler2D iChannel0;\n"
            "uniform sampler2D iChannel1;\n"
            "uniform sampler2D iChannel2;\n"
            "uniform sampler2D iChannel3;\n"
            "in vec2 vTexCoord;\n"
            "out vec4 fragmentColor;\n"
            "void main() {\n"
            "  vec2 uv = vTexCoord;\n"
            "  vec4 fragColor0 = vec4(texture(iChannel0, uv).rgb, 1.0);\n"
            "  vec4 fragColor1 = vec4(texture(iChannel1, uv).rgb, 1.0);\n"
            "  vec4 fragColor2 = vec4(texture(iChannel2, uv).rgb, 1.0);\n"
            "  vec4 fragColor3 = vec4(texture(iChannel3, uv).rgb, 1.0);\n"
            "  fragmentColor = mix(fragColor0, mix(fragColor1, mix(fragColor2, fragColor3, 0.5), 0.5), 0.5);\n"
            "}\n";

ImageMixer::ImageMixer(jni::JNIEnv &env, jni::jint width, jni::jint height)
{
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint eglMajVers, eglMinVers;
    eglInitialize(eglDisplay, &eglMajVers, &eglMinVers);
    LOGI("EGL init with version %d.%d", eglMajVers, eglMinVers);

    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint configAttr[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RED_SIZE,   8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE,  8,
            EGL_NONE
    };

    int iConfigs;
    EGLConfig eglConfig;
    eglChooseConfig(eglDisplay, configAttr, &eglConfig, 1, &iConfigs);

    if (iConfigs != 1) {
        jni::ThrowIllegalStateException(env, "Error: eglChooseConfig(): config not found");
    }

    const EGLint surfaceAttr[] = {
            EGL_WIDTH, width,
            EGL_HEIGHT, height,
            EGL_NONE
    };
    eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, surfaceAttr);

    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3, // Need to support GLSL 300
            EGL_NONE
    };
    eglContext = eglCreateContext(eglDisplay, eglConfig, nullptr,
                                  ctxAttr);

    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    LOGI("%s", glslVersion);

    is_egl_context_creator_thread = true;

    glProgram = createProgram(gVertexShader, gFragmentShader);
    if (!glProgram) {
        jni::ThrowIllegalStateException(env, "Could not create program.");
    }
    glPositionShaderInput = (GLuint)glGetAttribLocation(glProgram, "aPosition");
    checkGlError("glGetAttribLocation");

    glViewport(0, 0, width, height);
    checkGlError("glViewport");

    glGenBuffers(1, &glQuadBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, glQuadBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
}

ImageMixer::~ImageMixer()
{
    if (eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(eglDisplay, eglContext);
        }
        if (eglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(eglDisplay, eglSurface);
        }
        eglTerminate(eglDisplay);
    }

    eglDisplay = EGL_NO_DISPLAY;
    eglContext = EGL_NO_CONTEXT;
    eglSurface = EGL_NO_SURFACE;
}

void ImageMixer::loadTexture(jni::JNIEnv &env, jni::String &imagePath)
{
    if (!is_egl_context_creator_thread) {
        jni::ThrowIllegalStateException(env, "Called from thread which is different to one who created eglContext");
    }

    if (textures.size() >= MAX_IMAGES) {
        jni::ThrowIllegalStateException(env, "Attempt to add texture, but max supported textures exceed");
    }

    std::string image_path = jni::Make<std::string>(env, imagePath);
    GLuint tex = SOIL_load_OGL_texture(image_path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    checkGlError("glTexImage2D");

    if (!tex) {
        jni::ThrowIllegalStateException(env, "Image cannot be loaded");
    }

    textures.push_back(tex);
}

void ImageMixer::clearTextures(jni::JNIEnv &env)
{
    if (!is_egl_context_creator_thread) {
        jni::ThrowIllegalStateException(env, "Called from thread which is different to one who created eglContext");
    }

    for (auto tex : textures) {
        glDeleteTextures(1, &tex);
        checkGlError("glDeleteTextures");
    }

    textures.clear();
}

void ImageMixer::convert(jni::JNIEnv &env, jni::String &outPath)
{
    if (!is_egl_context_creator_thread) {
        jni::ThrowIllegalStateException(env, "Called from thread which is different to one who created eglContext");
    }

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    checkGlError("glClear");

    glUseProgram(glProgram);
    checkGlError("glUseProgram");

    // bind textures
    for (GLuint unit = 0; unit < MAX_IMAGES; unit++) {
        if (unit < textures.size()) {
            std::string uniformNameStr = "iChannel" + std::to_string(unit);
            GLint uniformName = glGetUniformLocation(glProgram, uniformNameStr.c_str());
            checkGlError("glGetUniformLocation");

            glActiveTexture(GL_TEXTURE0 + unit);
            checkGlError("glActiveTexture");
            glBindTexture(GL_TEXTURE_2D, textures[unit]);
            checkGlError("glBindTexture");
            glUniform1i(uniformName, unit);
            checkGlError("glUniform1i");

        }
    }


    glEnableVertexAttribArray(glPositionShaderInput);
    checkGlError("glEnableVertexAttribArray");

    glBindBuffer(GL_ARRAY_BUFFER, glQuadBuffer);
    checkGlError("glBindBuffer");
    glVertexAttribPointer(glPositionShaderInput, 2, GL_FLOAT, GL_FALSE, 0, 0);
    checkGlError("glVertexAttribPointer");

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    checkGlError("glDrawArrays");

    glDisableVertexAttribArray(glPositionShaderInput);
    checkGlError("glDisableVertexAttribArray");

    for (GLuint unit = 0; unit < MAX_IMAGES; unit++) {
        if (unit < textures.size()) {
            glActiveTexture(GL_TEXTURE0 + unit);
            checkGlError("glActiveTexture");
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            checkGlError("glBindTexture");
        }
    }

    // read
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const auto readPixels = std::get_temporary_buffer<uint32_t>(viewport[2] * viewport[3]);
    glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGB, GL_UNSIGNED_BYTE, readPixels.first);
    checkGlError("glReadPixels");

    glUseProgram(GL_NONE);
    checkGlError("glUseProgram");

    // save
    std::string result_path = jni::Make<std::string>(env, outPath);
    int success = SOIL_save_screenshot(result_path.c_str(), SOIL_SAVE_TYPE_BMP,
            viewport[0], viewport[1], viewport[2], viewport[3]);
    std::return_temporary_buffer(readPixels.first);

    if (!success) {
        jni::ThrowIllegalStateException(env, "Image cannot be saved to disk");
    }
}

};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    jni::JNIEnv& env { jni::GetEnv(*vm) };

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    using imgmixer::ImageMixer;

    jni::RegisterNativePeer<ImageMixer>(env,
            jni::Class<ImageMixer>::Find(env),
            "nativeHandle",
            jni::MakePeer<ImageMixer, jni::jint, jni::jint>,
            "initialize",
            "finalize",
            METHOD(&ImageMixer::loadTexture, "load0"),
            METHOD(&ImageMixer::clearTextures, "clear"),
            METHOD(&ImageMixer::convert, "convert0")
    );

#undef METHOD

    return jni::Unwrap(jni::jni_version_1_6);
}

//
// Created by CAMOBAP on 2019-05-05.
//

#pragma once

#include <jni/jni.hpp>

namespace jni {
    class IllegalStateException {
    public:
        static constexpr auto Name() { return "java/lang/IllegalStateException"; }
    };

    [[noreturn]] inline void ThrowIllegalStateException(JNIEnv &env, const char *message = nullptr) {
        ThrowNew(env, FindClass(env, IllegalStateException::Name()), message);
    }
};
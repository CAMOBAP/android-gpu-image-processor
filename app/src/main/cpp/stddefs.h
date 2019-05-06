#pragma once

#include <android/log.h>
#include <string.h>
#include <stdbool.h>

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, ANDROID_LOG_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, ANDROID_LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, ANDROID_LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, ANDROID_LOG_TAG, __VA_ARGS__))

//#define LOGD(...) ((void)printf("\n<D> " __VA_ARGS__))
//#define LOGI(...) ((void)printf("\n<I> " __VA_ARGS__))
//#define LOGW(...) ((void)printf("\n<W> " __VA_ARGS__))
//#define LOGE(...) ((void)printf("\n<E> " __VA_ARGS__))
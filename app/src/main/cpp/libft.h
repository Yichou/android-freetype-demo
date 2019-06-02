//
// Created by Yichou on 2019/6/1.
// freetype 调用接口封装
//

#ifndef MRPOID2018_LIBFT_H
#define MRPOID2018_LIBFT_H

#include <stdint.h>

#ifndef LOG_TAG
#include <android/log.h>

#define LOG_TAG "libft"
#define LOGI(...) \
    ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) \
    ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...) \
    ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#define LOGD(...) \
    ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif

uint8_t libft_init(int size);
void libft_setLcd(uint16_t* lcdBuf, uint16_t lcdW, uint16_t lcdH);
void libft_draWord(wchar_t word, int x, int y, uint16_t color);
void libft_draTextLeft(wchar_t *txt, int x, int y, uint16_t color, uint16_t size);

#endif //MRPOID2018_LIBFT_H

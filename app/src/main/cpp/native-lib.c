//
// Created by Yihcou on 2019/6/1.
//
#include <jni.h>
#include <stdint.h>
#include <android/bitmap.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "libft.h"
#include "libgh.h"

static uint16_t *ptr;
static uint32_t cw, ch;
int dx = 5, dy = 5;

long uptime() {
    struct timeval t = {0};

    gettimeofday(&t, NULL);
    if (t.tv_usec < 0) {
        t.tv_sec--;
        t.tv_usec += 1000000;
    }
    return (t.tv_sec * 1000 + t.tv_usec / 1000);
}

JNIEXPORT void JNICALL
Java_com_example_yicho_helloapp_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */ tz,
        jobject bmp) {
    AndroidBitmapInfo info;

    wchar_t *str = L"悦卓中国，Edroid.cn，1025";
//    wchar_t *str = L"鼎盛";
    wchar_t c = str[0];//'E';

    AndroidBitmap_getInfo(env, bmp, &info);
    AndroidBitmap_lockPixels(env, bmp, &ptr);
    cw = info.width;
    ch = info.height;

    libft_setLcd(ptr, cw, ch);
    libgh_setLcd(ptr, cw, ch);

    LOGE("bitmap w=%d h=%d p=%p", info.width, info.height, ptr);
    memset(ptr, 0xff, info.width*info.height*2);

//    libft_draWord(ptr, cw, c, dx, dy, 0x1133);
    long t0 = uptime();
    libgh_drawLineH(0, 50, cw, 0xf000);
    libft_draTextLeft(str, 10, 50, 0xf100, 20);

    libgh_drawLineH(0, 100, cw, 0xf000);
    libft_draTextLeft(str, 10, 100, 0xf100, 36);

    libgh_drawLineH(0, 200, cw, 0xf000);
    libft_draTextLeft(str, 10, 200, 0x0f10, 48);

    libgh_drawLineH(0, 300, cw, 0xf000);
    libft_draTextLeft(str, 10, 300, 0x001f, 60);
    LOGD("绘制时间 %dms", uptime() - t0);

    srand( (unsigned)time( NULL ) );
    uint16_t cl = rand();
//    drawRect(dx, dy, cw-dx*2, ch-dy*2, cl);

    dx += 5;
    dy += 5;

    AndroidBitmap_unlockPixels(env, bmp);
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    libft_init(36);
    return JNI_VERSION_1_4;
}


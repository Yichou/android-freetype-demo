//
// Created by Yihcou on 2019/6/1.
// 绘图接口封装
//

#ifndef HELLOAPP_LIBGH_H
#define HELLOAPP_LIBGH_H

#include <stdint.h>

void libgh_setLcd(uint16_t* lcdBuf, uint16_t lcdW, uint16_t lcdH);

void libgh_drawRect(int x, int y, int w, int h, uint16_t c);

void libgh_drawLine(int x1, int y1, int x2, int y2, uint16_t c);

#define libgh_drawLineH(x, y, w, c) \
    libgh_drawLine(x, y, x+w, y, c)

#define libgh_drawLineV(x, y, h, c) \
    libgh_drawLine(x, y, x, y+h, c)

#endif //HELLOAPP_LIBGH_H

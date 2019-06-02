//
// Created by yicho on 2019/6/1.
// 绘图接口封装
//
#include "libgh.h"

static uint16_t *g_lcdBuf;
static uint16_t g_lcdW, g_lcdH;

void libgh_setLcd(uint16_t* lcdBuf, uint16_t lcdW, uint16_t lcdH) {
    g_lcdBuf = lcdBuf;
    g_lcdW = lcdW;
    g_lcdH = lcdH;
}

void libgh_drawLine(int x1, int y1, int x2, int y2, uint16_t c) {
    int x, y, dx, dy, s1, s2, p, temp, int32erchange, i;

    x = x1;
    y = y1;
    dx = x2 > x1 ? (x2 - x1) : (x1 - x2);
    dy = y2 > y1 ? (y2 - y1) : (y1 - y2);

    s1 = x2 > x1 ? 1 : -1;
    s2 = y2 > y1 ? 1 : -1;

    if (dy > dx) {
        temp = dx;
        dx = dy;
        dy = temp;
        int32erchange = 1;
    } else
        int32erchange = 0;

    p = (dy << 1) - dx;
    for (i = 0; i <= dx; i++) {
        if ((x >= 0 && x < g_lcdW) && (y >= 0 && y <= g_lcdH)) {
            *(g_lcdBuf + g_lcdW * y + x) = c; //SETSCNP(x, y, dc);
        }

        //dian(x, y);
        if (p >= 0) {
            if (!int32erchange)
                y = y + s2;
            else
                x = x + s1;
            p = p - (dx << 1);
        }
        if (!int32erchange)
            x = x + s1;
        else
            y = y + s2;
        p = p + (dy << 1);
    }
}

void libgh_drawRect(int x, int y, int w, int h, uint16_t c) {
    libgh_drawLine(x, y, x+w, y, c);
    libgh_drawLine(x+w, y, x+w, y+h, c);
    libgh_drawLine(x+w, y+h, x, y+h, c);
    libgh_drawLine(x, y+h, x, y, c);
    libgh_drawLine(x, y, x+w, y+h, c);
    libgh_drawLine(x, y+h, x+w, y, c);
}
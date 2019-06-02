//
// Created by Yichou on 2019/6/1.
// freetype 调用接口封装
//
#include "libft.h"

#include <stdlib.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <math.h>
#include <wchar.h>

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} T_COLOR;


static int error; //this flag is set to true if any error occurs during freetype initialization

static FT_Library library;
static FT_Face fontFace;
static int resolution, glyphMargin;
static uint16_t* texture;
static uint16_t texW, texH;


static int nextPowerOf2(int a) {
    int rval = 1;
    while (rval < a) rval *= 2;
    return rval;
}

static void rgb565_to_color(int src, T_COLOR *ct) {
    ct->r = ((((src >> 11) & 0x1F) * 527) + 23) >> 6;
    ct->g = ((((src >> 5) & 0x3F) * 259) + 33) >> 6;
    ct->b = (((src & 0x1F) * 527) + 23) >> 6;
}

#define RGB8882RGB565(rgb888) \
    ((((rgb888)&0xf80000)>>8)|(((rgb888)&0xfc00)>>5)|(((rgb888)&0xf8)>>3))

#define C565R(pixel)      (uint8_t)(((pixel) >> 11) & 0x1f)
#define C565G(pixel)      (uint8_t)(((pixel) >> 5) & 0x3f)
#define C565B(pixel)      (uint8_t)((pixel) & 0x1f)

/* create 16 bit 5/6/5 format pixel from RGB triplet */
#define RGB2PIXEL565(r,g,b)    \
	((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3))

static void setPixel(int x, int y, uint8_t val, uint16_t color) {
    uint16_t src = texture[x + y * texW];

    register uint32_t d1; // 计算用的中间变量，声明为寄存器变量快些
    register uint16_t wa = src; // 源颜色
    register uint16_t wb = color; // 目的颜色
    register uint32_t alpha = 32-(0.125*val); // alpha值，16位色的色深为32级，

    d1 = (((((((wa << 16) | wa) & 0x7e0f81f) - (((wb << 16) | wb) & 0x7e0f81f))* alpha) >> 5) + (((wb << 16) | wb) & 0x7e0f81f)) & 0x7e0f81f;
    wa = (d1 & 0xffff0000)>>16; // g...r...b => ..g..
    wb = d1 & 0xffff; // g...r...b => r...b
    uint16_t dst = wa | wb; // rgb

    texture[x + y * texW] = dst;
}

//Copy the bitmap of the given glyph to the texture atlas at the location (atlasX, atlasY). If drawBorder is true, a white border is drawn around the glyph square
static void
copyGlyphToTex(FT_GlyphSlot glyph,
        int x, int y,
        uint16_t color,
        uint8_t drawBorder) {
    //The bitmap might be smaller than our square of [squareSize*squareSize]. So we first
    //copy the glyph bitmap and then add black padding if needed
    const int gr = glyph->bitmap.rows;
    const int gw = glyph->bitmap.width;

    x += glyph->bitmap_left;
    y -= glyph->bitmap_top;

    for (int h = 0; h < gr; h++) {
        for (int w = 0; w < gw; w++) {
            //Need to add the top margin (marginSize*texSize offset) and the left margin (marginSize)
//            setPixel(texture, x + w, y + h, texWidth, glyph->bitmap.buffer[w + h * gw], color);
            uint8_t g = glyph->bitmap.buffer[w + h * gw];
            if (g > 0) {
                if(x + w >= texW || y + h >= texH)
                    continue;

//                texture[(y + h) * texW + x + w] = color;
                setPixel(x+w, y+h, g, color);
            }
        }
    }

#if 0
    if (!drawBorder) { //with borders, padding has already been done
        for (int h=0; h<squareSize; h++) {
            if (h < glyphMargin || h >= glyphMargin+gr) {
                //bottom padding => whole rows
                for (int w=0; w<squareSize; w++) {
                    setPixel(texture, baseOffset, texSize, w, h, 0);
                }
            } else {
                //left padding
                for (int w=0; w<glyphMargin; w++)
                    setPixel(texture, baseOffset, texSize, w, h, 0);
                //right padding
                for (int w=gw+glyphMargin; w<squareSize; w++)
                    setPixel(texture, baseOffset, texSize, w, h, 0);
            }
        }
    }
#endif
}

uint8_t libft_load_file(const char *path) {
    //TODO: To avoid loading ALL glyphs from a particular typeFace, we might
    //let the user provides one or more "character ranges" that we should load

    //TODO: If we have a font face with a HUGE amount of glyphs, maybe it would
    //be better to generate multiple atlas (and a glyph would be found by using
    //its atlas coordinates AND an atlas index)
    if (FT_New_Face(library, path, 0, &fontFace)) {
        LOGE("Error loading font face %s", path);
        error = 2;
        return error;
    }

    //Each character will be rendered in a square of resolution*resolution pixels
    FT_Set_Pixel_Sizes(fontFace, resolution, resolution);

    LOGD("libft_load_file %s face=%p num_glyphs=%d family_name=%s",
         path, fontFace, fontFace->num_glyphs, fontFace->family_name);

    return 0;
}

uint8_t libft_load_mem(const char *memBase, size_t memLength) {
    if (FT_New_Memory_Face(library, (FT_Byte *) memBase, (FT_Long) memLength, 0, &fontFace)) {
        LOGE("Error loading font face");
        error = 2;
        return error;
    }

    //Each character will be rendered in a square of resolution*resolution pixels
    FT_Set_Pixel_Sizes(fontFace, resolution, 0);

    return 0;
}

uint8_t libft_init(int size) {
    if (FT_Init_FreeType(&library)) {
        LOGE("Failed to initialize freetype library");
        error = 1;
        return error;
    }

    LOGW("==========libft_init ok size=%d lib=%p", size, library);

    resolution = size;
    glyphMargin = 0;

//    libft_load_file("/system/fonts/Roboto-Black.ttf");
    libft_load_file("/data/local/tmp/fzlt.ttf");

    return error;
}

void libft_setLcd(uint16_t* lcdBuf, uint16_t lcdW, uint16_t lcdH) {
    texture = lcdBuf;
    texW = lcdW;
    texH = lcdH;
}

/**
 * 往565画布画文字
 *
 * @param texture
 * @param texWidth
 * @param word
 * @param x
 * @param y
 * @param color
 */
void libft_draWord(wchar_t word, int x, int y, uint16_t color) {

    /*
   * We build a single texture (the "atlas") to store all our glyphs. Each glyph
   * has an atlasCoord, which represent its position in the atlas (NOT in pixels,
   * but in glyphs).
   * For example, G1 will have the atlas coords (0,0), G2(1,0) and G20(0,1)
   ------------------------------
   |G1|G2|G3|.......            |
   |----------------------------|
   |G20|                        |
   |                            |
   |                       |Gn-1|
   ------------------------------
   |Gn|                         |
   ------------------------------
   */

    //The actual size of a side of the square that will contain our glyph
    const int squareSize = resolution + glyphMargin * 2;

    //we use two channels (one for luminance, one for alpha)
//    uint8_t textureData[realTexSize * realTexSize * 2] = {0};

    if (FT_Load_Char(fontFace, word, FT_LOAD_RENDER)) {
        LOGE("Error loading char %c", word);
        return;
    }

    FT_GlyphSlot glyph = fontFace->glyph;
//    FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);

    //Calculate glyph informations
    float leftMargin = glyph->bitmap_left / (float) squareSize;
    float topMargin = -(glyph->bitmap_top / (float) squareSize);
//    float atlasX = texAtlasX*squareSize/(float)realTexSize;
    //add 1 to avoid border aliasing issues
//    float atlasY = (texAtlasY*squareSize+1)/(float)realTexSize;
    //advance is stored in fractional pixel format (=1/64 pixels)
    float advance = (glyph->advance.x) / (64.0f * squareSize);

//    glyphs.insert(charcode, glyphInfo);

    LOGI("Load Glyph %c l=%d t=%d h=%d w=%d pm=%d",
         word, glyph->bitmap_left, glyph->bitmap_top, glyph->bitmap.rows, glyph->bitmap.width,
         glyph->bitmap.pixel_mode);

    //Copy the bitmap to the atlas
    copyGlyphToTex(glyph, x, y, color, 0);
}

void libft_draTextLeft(wchar_t *txt, int x, int y, uint16_t color, uint16_t size) {
    if(size > 0 && size != resolution) {
        resolution = size;
        FT_Set_Pixel_Sizes(fontFace, resolution, 0);
    }

    int n = wcslen(txt);
    int i;

    for(i=0; i<n; i++) {
        wchar_t c = txt[i];
        if (FT_Load_Char(fontFace, c, FT_LOAD_DEFAULT)) {
            LOGE("Error loading char %c", c);
            continue;
        }

        FT_GlyphSlot glyph = fontFace->glyph;
        FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);
//        LOGI("Load Glyph %c l=%d t=%d h=%d w=%d pm=%d",
//             c, glyph->bitmap_left, glyph->bitmap_top, glyph->bitmap.rows, glyph->bitmap.width, glyph->bitmap.pixel_mode);

        copyGlyphToTex(glyph, x, y, color, 0);

        x += glyph->bitmap.width + glyph->bitmap_left;
    }
}

void libft_dsipose() {
    FT_Done_FreeType(library);
}

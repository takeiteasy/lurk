//
//  image.c
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#include "wee.h"

Color RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return (Color) {
        .r = r,
        .g = g,
        .b = b,
        .a = a
    };
}

Color RGB(unsigned char r, unsigned char g, unsigned char b) {
    return RGBA(r, g, b, 255);
}

Color RGBA1(unsigned char c, unsigned char a) {
    return RGBA(c, c, c, a);
}

Color RGB1(unsigned char c) {
    return RGB(c, c, c);
}

Image* CreateImage(unsigned int w, unsigned int h) {
    Image *img = malloc(sizeof(img));
    img->w = w;
    img->h = h;
    size_t sz = w * h * sizeof(unsigned int) + 1;
    img->buf = malloc(sz);
    memset(img->buf, 0, sz);
    return img;
}

void DestroyImage(Image *img) {
    if (img) {
        if (img->buf)
            free(img->buf);
        free(img);
    }
}

void FillImage(Image *img, Color col) {
    for (int i = 0; i < img->w * img->h; ++i)
        img->buf[i] = col.value;
}

#define CmpColor(A, B) ((A).value == (B).value)

static inline void flood_fn(Image *img, int x, int y, Color new, Color old) {
    if (CmpColor(new, old) || !CmpColor(PGet(img, x, y), old))
        return;
    
    int x1 = x;
    while (x1 < img->w && CmpColor(PGet(img, x1, y), old)) {
        PSet(img, x1, y, new);
        x1++;
    }
    
    x1 = x - 1;
    while (x1 >= 0 && CmpColor(PGet(img, x1, y), old)) {
        PSet(img, x1, y, new);
        x1--;
    }
    
    x1 = x;
    while (x1 < img->w && CmpColor(PGet(img, x1, y), new)) {
        if(y > 0 && CmpColor(PGet(img, x1, y - 1), old))
            flood_fn(img, x1, y - 1, new, old);
        x1++;
    }
    
    x1 = x - 1;
    while(x1 >= 0 && CmpColor(PGet(img, x1, y), new)) {
        if(y > 0 && CmpColor(PGet(img, x1, y - 1), old))
            flood_fn(img, x1, y - 1, new, old);
        x1--;
    }
    
    x1 = x;
    while(x1 < img->w && CmpColor(PGet(img, x1, y), new)) {
        if(y < img->h - 1 && CmpColor(PGet(img, x1, y + 1), old))
            flood_fn(img, x1, y + 1, new, old);
        x1++;
    }
    
    x1 = x - 1;
    while(x1 >= 0 && CmpColor(PGet(img, x1, y), new)) {
        if(y < img->h - 1 && CmpColor(PGet(img, x1, y + 1), old))
            flood_fn(img, x1, y + 1, new, old);
        x1--;
    }
}

void FloodImage(Image *img, int x, int y, Color col) {
    if (x < 0 || y < 0 || x >= img->w || y >= img->h)
        return;
    flood_fn(img, x, y, col, PGet(img, x, y));
}

void ClearImage(Image *img) {
    memset(img->buf, 0, img->w * img->h * sizeof(int));
}

#define BLEND(c0, c1, a0, a1) (c0 * a0 / 255) + (c1 * a1 * (255 - a0) / 65025)

void BSet(Image *img, int x, int y, Color col) {
    if (col.a == 255) {
        PSet(img, x, y, col);
        return;
    }
    Color old = { .value = img->buf[y * img->w + x] };
    Color new = {
        .r = BLEND(col.r, old.r, col.a, old.a),
        .g = BLEND(col.g, old.g, col.g, old.g),
        .b = BLEND(col.b, old.b, col.b, old.b),
        .a = col.a + (old.a * (255 - col.a) >> 8)
    };
    img->buf[y * img->w + x] = new.value;
}

void PSet(Image *img, int x, int y, Color col) {
    img->buf[y * img->w + x] = col.value;
}

Color PGet(Image *img, int x, int y) {
    return (Color) { .value = img->buf[y * img->w + x] };
}

void PasteImage(Image *dst, Image *src, int x, int y) {
    Color col;
    for (int ox = 0; ox < src->w; ++ox) {
        for (int oy = 0; oy < src->h; ++oy) {
            if (oy > dst->h)
                break;
            col = PGet(src, ox, oy);
            BSet(dst, x + ox, y + oy, col);
        }
        if (ox > dst->w)
            break;
    }
}

void PasteImageClip(Image *dst, Image *src, int x, int y, int rx, int ry, int rw, int rh) {
    for (int ox = 0; ox < rw; ++ox)
        for (int oy = 0; oy < rh; ++oy)
            BSet(dst, ox + x, oy + y, PGet(src, ox + rx, oy + ry));
}

Image* CopyImage(Image *src) {
    Image *img = CreateImage(src->w, src->h);
    memcpy(img->buf, src->buf, src->w * src->h * sizeof(unsigned int));
    return img;
}

void PassthruImage(Image *img, int (*fn)(int x, int y, Color col)) {
    int x, y;
    for (x = 0; x < img->w; ++x)
        for (y = 0; y < img->h; ++y)
            img->buf[y * img->w + x] = fn(x, y, PGet(img, x, y));
}

Image* ScaleImage(Image *src, int nw, int nh) {
    Image *img = CreateImage(nw, nh);
    int x_ratio = (int)((src->w << 16) / img->w) + 1;
    int y_ratio = (int)((src->h << 16) / img->h) + 1;
    int x2, y2, i, j;
    for (i = 0; i < img->h; ++i) {
        int *t = img->buf + i * img->w;
        y2 = ((i * y_ratio) >> 16);
        int *p = src->buf + y2 * src->w;
        int rat = 0;
        for (j = 0; j < img->w; ++j) {
            x2 = (rat >> 16);
            *t++ = p[x2];
            rat += x_ratio;
        }
    }
    return img;
}

#define __MIN(a, b) (((a) < (b)) ? (a) : (b))
#define __MAX(a, b) (((a) > (b)) ? (a) : (b))
#define __D2R(a) ((a) * M_PI / 180.0)

Image* RotateImage(Image *src, float angle) {
    float theta = __D2R(angle);
    float c = cosf(theta), s = sinf(theta);
    float r[3][2] = {
        { -src->h * s, src->h * c },
        {  src->w * c - src->h * s, src->h * c + src->w * s },
        {  src->w * c, src->w * s }
    };
    
    float mm[2][2] = {{
        __MIN(0, __MIN(r[0][0], __MIN(r[1][0], r[2][0]))),
        __MIN(0, __MIN(r[0][1], __MIN(r[1][1], r[2][1])))
    }, {
        (theta > 1.5708  && theta < 3.14159 ? 0.f : __MAX(r[0][0], __MAX(r[1][0], r[2][0]))),
        (theta > 3.14159 && theta < 4.71239 ? 0.f : __MAX(r[0][1], __MAX(r[1][1], r[2][1])))
    }};
    
    int dw = (int)ceil(fabsf(mm[1][0]) - mm[0][0]);
    int dh = (int)ceil(fabsf(mm[1][1]) - mm[0][1]);
    Image *img = CreateImage(dw, dh);
    
    int x, y, sx, sy;
    for (x = 0; x < dw; ++x)
        for (y = 0; y < dh; ++y) {
            sx = ((x + mm[0][0]) * c + (y + mm[0][1]) * s);
            sy = ((y + mm[0][1]) * c - (x + mm[0][0]) * s);
            if (sx < 0 || sx >= src->w || sy < 0 || sy >= src->h)
                continue;
            BSet(img, x, y, PGet(src, sx, sy));
        }
    return img;
}

static inline void vline(Image *img, int x, int y0, int y1, Color col) {
    if (y1 < y0) {
        y0 += y1;
        y1  = y0 - y1;
        y0 -= y1;
    }
    
    if (x < 0 || x >= img->w || y0 >= img->h)
        return;
    
    if (y0 < 0)
        y0 = 0;
    if (y1 >= img->h)
        y1 = img->h - 1;
    
    for(int y = y0; y <= y1; y++)
        BSet(img, x, y, col);
}

static inline void hline(Image *img, int y, int x0, int x1, Color col) {
    if (x1 < x0) {
        x0 += x1;
        x1  = x0 - x1;
        x0 -= x1;
    }
    
    if (y < 0 || y >= img->h || x0 >= img->w)
        return;
    
    if (x0 < 0)
        x0 = 0;
    if (x1 >= img->w)
        x1 = img->w - 1;
    
    for(int x = x0; x <= x1; x++)
        BSet(img, x, y, col);
}

void DrawLine(Image *img, int x0, int y0, int x1, int y1, Color col) {
    if (x0 == x1)
        vline(img, x0, y0, y1, col);
    if (y0 == y1)
        hline(img, y0, x0, x1, col);
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    
    while (BSet(img, x0, y0, col), x0 != x1 || y0 != y1) {
        int e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 <  dy) { err += dx; y0 += sy; }
    }
}

void DrawCircle(Image *img, int xc, int yc, int r, Color col, bool fill) {
    int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
    do {
        BSet(img, xc - x, yc + y, col);    /*   I. Quadrant */
        BSet(img, xc - y, yc - x, col);    /*  II. Quadrant */
        BSet(img, xc + x, yc - y, col);    /* III. Quadrant */
        BSet(img, xc + y, yc + x, col);    /*  IV. Quadrant */
        
        if (fill) {
            hline(img, yc - y, xc - x, xc + x, col);
            hline(img, yc + y, xc - x, xc + x, col);
        }
        
        r = err;
        if (r <= y)
            err += ++y * 2 + 1; /* e_xy+e_y < 0 */
        if (r > x || err > y)
            err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);
}

void DrawRect(Image *img, int x, int y, int w, int h, Color col, bool fill) {
    if (x < 0) {
        w += x;
        x  = 0;
    }
    if (y < 0) {
        h += y;
        y  = 0;
    }
    
    w += x;
    h += y;
    if (w < 0 || h < 0 || x > img->w || y > img->h)
        return;
    
    if (w > img->w)
        w = img->w;
    if (h > img->h)
        h = img->h;
    
    if (fill) {
        for (; y < h; ++y)
            hline(img, y, x, w, col);
    } else {
        hline(img, y, x, w, col);
        hline(img, h, x, w, col);
        vline(img, x, y, h, col);
        vline(img, w, y, h, col);
    }
}

#define __SWAP(a, b)  \
    do                \
    {                 \
        int temp = a; \
        a = b;        \
        b = temp;     \
    } while (0)

void DrawTri(Image *img, int x0, int y0, int x1, int y1, int x2, int y2, Color col, bool fill) {
    if (y0 ==  y1 && y0 ==  y2)
        return;
    if (fill) {
        if (y0 > y1) {
            __SWAP(x0, x1);
            __SWAP(y0, y1);
        }
        if (y0 > y2) {
            __SWAP(x0, x2);
            __SWAP(y0, y2);
        }
        if (y1 > y2) {
            __SWAP(x1, x2);
            __SWAP(y1, y2);
        }
        
        int total_height = y2 - y0, i, j;
        for (i = 0; i < total_height; ++i) {
            bool second_half = i > y1 - y0 || y1 == y0;
            int segment_height = second_half ? y2 - y1 : y1 - y0;
            float alpha = (float)i / total_height;
            float beta  = (float)(i - (second_half ? y1 - y0 : 0)) / segment_height;
            int ax = x0 + (x2 - x0) * alpha;
            int ay = y0 + (y2 - y0) * alpha;
            int bx = second_half ? x1 + (x2 - x1) : x0 + (x1 - x0) * beta;
            int by = second_half ? y1 + (y2 - y1) : y0 + (y1 - y0) * beta;
            if (ax > bx) {
                __SWAP(ax, bx);
                __SWAP(ay, by);
            }
            for (j = ax; j <= bx; ++j)
                BSet(img, j, y0 + i, col);
        }
    } else {
        DrawLine(img, x0, y0, x1, y1, col);
        DrawLine(img, x1, y1, x2, y2, col);
        DrawLine(img, x2, y2, x0, y0, col);
    }
}

static char* FileExt(const char *path) {
    char *dot = strrchr(path, '.');
    return !dot || dot == path ? NULL : dot + 1;
}

static Image* ImageFromFormattedMemory(unsigned char *data, int w, int h, int channels) {
    Image *result = CreateImage(w, h);
    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++) {
            unsigned char *p = data + (x + w * y) * channels;
            PSet(result, x, y, RGBA(p[0], p[1], p[2], channels == 4 ? p[3] : 255));
        }
    return result;
}

Image* LoadImage(const char *path) {
    int w, h, c;
    unsigned char *data = NULL;
    char *ext = FileExt(path);
    if (!strncmp(ext, "qoi", 3)) {
        qoi_desc desc;
        data = qoi_read(path, &desc, 0);
        c = desc.channels;
        w = desc.width;
        h = desc.height;
    } else
        data = stbi_load(path, &w, &h, &c, 4);
    assert(data);
    
    Image *result = ImageFromFormattedMemory(data, w, h, c);
    free(data);
    return result;
}

Image* LoadImageMemory(const void *data, size_t sizeOfData) {
    int w, h, c;
    unsigned char *fdata = stbi_load_from_memory((const stbi_uc*)data, (int)sizeOfData, &w, &h, &c, 4);
    Image *result = ImageFromFormattedMemory(fdata, w, h, c);
    free(fdata);
    return result;
}

#if !defined(WEE_JPEG_QUALITY)
#define WEE_JPEG_QUALITY 80
#endif

void SaveImage(Image *img, const char *path) {
    char *ext = FileExt(path);
    size_t sizeOfData = img->w * img->h * 4 * sizeof(char);
    char *data = malloc(sizeOfData);
    for (int i = 0, x = 0; x < img->h; x++)
        for (int y = 0; y < img->w; y++) {
            Color c = PGet(img, y, x);
            data[i++] = c.r;
            data[i++] = c.g;
            data[i++] = c.b;
            data[i++] = c.a;
        }
    
    if (!strncmp(ext, "png", 3))
        stbi_write_png(path, img->w, img->h, 4, data, 0);
    else if (!strncmp(ext, "bmp", 3))
        stbi_write_bmp(path, img->w, img->h, 4, data);
    else if (!strncmp(ext, "tga", 3))
        stbi_write_tga(path, img->w, img->h, 4, data);
    else if (!strncmp(ext, "jpg", 3))
        stbi_write_jpg(path, img->w, img->h, 4, data, WEE_JPEG_QUALITY);
    else if (!strncmp(ext, "qoi", 3)) {
        qoi_desc desc = {
            .width = img->w,
            .height = img->h,
            .channels = 4,
            .colorspace = QOI_SRGB
        };
        qoi_write(path, data, &desc);
    } else {
        FILE *fh = fopen(path, "wb");
        assert(fh);
        fwrite(data, sizeOfData, 1, fh);
        fclose(fh);
    }
    free(data);
}

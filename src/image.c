//
//  image.c
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#include "wee.h"

int RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return ((unsigned int)a << 24) | ((unsigned int)r << 16) | ((unsigned int)g << 8) | b;
}

int RGB(unsigned char r, unsigned char g, unsigned char b) {
    return RGBA(r, g, b, 255);
}

int RGBA1(unsigned char c, unsigned char a) {
    return RGBA(c, c, c, a);
}

int RGB1(unsigned char c) {
    return RGB(c, c, c);
}

unsigned char Rgba(int c) {
    return (unsigned char)((c >> 16) & 0xFF);
}

unsigned char rGba(int c) {
    return (unsigned char)((c >>  8) & 0xFF);
}

unsigned char rgBa(int c) {
    return (unsigned char)(c & 0xFF);
}

unsigned char rgbA(int c) {
    return (unsigned char)((c >> 24) & 0xFF);
}

int rGBA(int c, unsigned char r) {
    return (c & ~0x00FF0000) | (r << 16);
}

int RgBA(int c, unsigned char g) {
    return (c & ~0x0000FF00) | (g << 8);
}

int RGbA(int c, unsigned char b) {
    return (c & ~0x000000FF) | b;
}

int RGBa(int c, unsigned char a) {
    return (c & ~0x00FF0000) | (a << 24);
}

bool InitImage(Image *img, unsigned int w, unsigned int h) {
    img->w = w;
    img->h = h;
    size_t sz = w * h * sizeof(unsigned int) + 1;
    img->buf = malloc(sz);
    memset(img->buf, 0, sz);
    return true;
}

void DestroyImage(Image *img) {
    if (img->buf)
        free(img->buf);
    memset(img, 0, sizeof(Image));
}

void FillImage(Image *img, int col) {
    for (int i = 0; i < img->w * img->h; ++i)
        img->buf[i] = col;
}

static inline void flood_fn(Image *img, int x, int y, int new, int old) {
    if (new == old || PGet(img, x, y) != old)
        return;
    
    int x1 = x;
    while (x1 < img->w && PGet(img, x1, y) == old) {
        PSet(img, x1, y, new);
        x1++;
    }
    
    x1 = x - 1;
    while (x1 >= 0 && PGet(img, x1, y) == old) {
        PSet(img, x1, y, new);
        x1--;
    }
    
    x1 = x;
    while (x1 < img->w && PGet(img, x1, y) == new) {
        if(y > 0 && PGet(img, x1, y - 1) == old)
            flood_fn(img, x1, y - 1, new, old);
        x1++;
    }
    
    x1 = x - 1;
    while(x1 >= 0 && PGet(img, x1, y) == new) {
        if(y > 0 && PGet(img, x1, y - 1) == old)
            flood_fn(img, x1, y - 1, new, old);
        x1--;
    }
    
    x1 = x;
    while(x1 < img->w && PGet(img, x1, y) == new) {
        if(y < img->h - 1 && PGet(img, x1, y + 1) == old)
            flood_fn(img, x1, y + 1, new, old);
        x1++;
    }
    
    x1 = x - 1;
    while(x1 >= 0 && PGet(img, x1, y) == new) {
        if(y < img->h - 1 && PGet(img, x1, y + 1) == old)
            flood_fn(img, x1, y + 1, new, old);
        x1--;
    }
}

void FloodImage(Image *img, int x, int y, int col) {
    if (x < 0 || y < 0 || x >= img->w || y >= img->h)
        return;
    flood_fn(img, x, y, col, PGet(img, x, y));
}

void ClearImage(Image *img) {
    memset(img->buf, 0, img->w * img->h * sizeof(int));
}

#define BLEND(c0, c1, a0, a1) (c0 * a0 / 255) + (c1 * a1 * (255 - a0) / 65025)

void BSet(Image *img, int x, int y, int c) {
    unsigned char a = rgbA(c);
    if (!a || x < 0 || y < 0 || x >= img->w || y >= img->h)
        return;
    if (a == 255) {
        PSet(img, x, y, c);
        return;
    }
    int *p = &img->buf[y * img->w + x];
    unsigned char _b = rgbA(*p);
    *p = (a == 255 || !img) ? c : RGBA(BLEND(Rgba(c), Rgba(*p), a, _b),
                                       BLEND(rGba(c), rGba(*p), a, _b),
                                       BLEND(rgBa(c), rgBa(*p), a, _b),
                                       a + (_b * (255 - a) >> 8));
}

void PSet(Image *img, int x, int y, int col) {
    if (x >= 0 && y >= 0 && x < img->w && y < img->h)
        img->buf[y * img->w + x] = col;
}

int PGet(Image *img, int x, int y) {
    return (x >= 0 && y >= 0 && x < img->w && y < img->h) ? img->buf[y * img->w + x] : 0;
}

bool PasteImage(Image *dst, Image *src, int x, int y) {
    int ox, oy, c;
    for (ox = 0; ox < src->w; ++ox) {
        for (oy = 0; oy < src->h; ++oy) {
            if (oy > dst->h)
                break;
            c = PGet(src, ox, oy);
            BSet(dst, x + ox, y + oy, c);
        }
        if (ox > dst->w)
            break;
    }
    return true;
}

bool PasteImageClip(Image *dst, Image *src, int x, int y, int rx, int ry, int rw, int rh) {
    for (int ox = 0; ox < rw; ++ox)
        for (int oy = 0; oy < rh; ++oy)
            BSet(dst, ox + x, oy + y, PGet(src, ox + rx, oy + ry));
    return true;
}

bool CopyImage(Image *a, Image *img) {
    if (!InitImage(img, a->w, a->h))
        return false;
    memcpy(img->buf, a->buf, a->w * a->h * sizeof(unsigned int) + 1);
    return !!img->buf;
}

void PassthruImage(Image *img, int (*fn)(int x, int y, int col)) {
    int x, y;
    for (x = 0; x < img->w; ++x)
        for (y = 0; y < img->h; ++y)
            img->buf[y * img->w + x] = fn(x, y, PGet(img, x, y));
}

bool ScaleImage(Image *a, int nw, int nh, Image *img) {
    if (!InitImage(img, nw, nh))
        return false;
    
    int x_ratio = (int)((a->w << 16) / img->w) + 1;
    int y_ratio = (int)((a->h << 16) / img->h) + 1;
    int x2, y2, i, j;
    for (i = 0; i < img->h; ++i) {
        int *t = img->buf + i * img->w;
        y2 = ((i * y_ratio) >> 16);
        int *p = a->buf + y2 * a->w;
        int rat = 0;
        for (j = 0; j < img->w; ++j) {
            x2 = (rat >> 16);
            *t++ = p[x2];
            rat += x_ratio;
        }
    }
    return true;
}

#define __MIN(a, b) (((a) < (b)) ? (a) : (b))
#define __MAX(a, b) (((a) > (b)) ? (a) : (b))
#define __D2R(a) ((a) * M_PI / 180.0)

bool RotateImage(Image *a, float angle, Image *img) {
    float theta = __D2R(angle);
    float c = cosf(theta), s = sinf(theta);
    float r[3][2] = {
        { -a->h * s, a->h * c },
        {  a->w * c - a->h * s, a->h * c + a->w * s },
        {  a->w * c, a->w * s }
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
    if (!InitImage(img, dw, dh))
        return false;
    
    int x, y, sx, sy;
    for (x = 0; x < dw; ++x)
        for (y = 0; y < dh; ++y) {
            sx = ((x + mm[0][0]) * c + (y + mm[0][1]) * s);
            sy = ((y + mm[0][1]) * c - (x + mm[0][0]) * s);
            if (sx < 0 || sx >= a->w || sy < 0 || sy >= a->h)
                continue;
            BSet(img, x, y, PGet(a, sx, sy));
        }
    return true;
}

static inline void vline(Image *img, int x, int y0, int y1, int col) {
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

static inline void hline(Image *img, int y, int x0, int x1, int col) {
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

void DrawLine(Image *img, int x0, int y0, int x1, int y1, int col) {
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

void DrawCircle(Image *img, int xc, int yc, int r, int col, bool fill) {
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

void DrawRect(Image *img, int x, int y, int w, int h, int col, bool fill) {
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

void DrawTri(Image *img, int x0, int y0, int x1, int y1, int x2, int y2, int col, bool fill) {
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

typedef struct {
    const unsigned char *p, *end;
} PNG;

static unsigned get32(const unsigned char* v) {
    return (v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
}

static const unsigned char* find(PNG* png, const char* chunk, unsigned minlen) {
    const unsigned char* start;
    while (png->p < png->end) {
        unsigned len = get32(png->p + 0);
        start = png->p;
        png->p += len + 12;
        if (memcmp(start + 4, chunk, 4) == 0 && len >= minlen && png->p <= png->end)
            return start + 8;
    }
    return NULL;
}

static unsigned char paeth(unsigned char a, unsigned char b, unsigned char c) {
    int p = a + b - c;
    int pa = abs(p - a), pb = abs(p - b), pc = abs(p - c);
    return (pa <= pb && pa <= pc) ? a : (pb <= pc) ? b : c;
}

static int rowBytes(int w, int bipp) {
    int rowBits = w * bipp;
    return rowBits / 8 + ((rowBits % 8) ? 1 : 0);
}

static int unfilter(int w, int h, int bipp, unsigned char* raw) {
    int len = rowBytes(w, bipp);
    int bpp = rowBytes(1, bipp);
    int x, y;
    unsigned char* first = (unsigned char*)malloc(len + 1);
    memset(first, 0, len + 1);
    unsigned char* prev = first;
    for (y = 0; y < h; y++, prev = raw, raw += len) {
#define LOOP(A, B)            \
    for (x = 0; x < bpp; x++) \
        raw[x] += A;          \
    for (; x < len; x++)      \
        raw[x] += B;          \
    break
        switch (*raw++) {
            case 0:
                break;
            case 1:
                LOOP(0, raw[x - bpp]);
            case 2:
                LOOP(prev[x], prev[x]);
            case 3:
                LOOP(prev[x] / 2, (raw[x - bpp] + prev[x]) / 2);
            case 4:
                LOOP(prev[x], paeth(raw[x - bpp], prev[x], prev[x - bpp]));
            default:
                return 0;
        }
#undef LOOP
    }
    free(first);
    return 1;
}

static void convert(int bypp, int w, int h, const unsigned char* src, int* dest, const unsigned char* trns) {
    int x, y;
    for (y = 0; y < h; y++) {
        src++;  // skip filter byte
        for (x = 0; x < w; x++, src += bypp) {
            switch (bypp) {
                case 1: {
                    unsigned char c = src[0];
                    if (trns && c == *trns) {
                        *dest++ = RGBA1(c, 0);
                        break;
                    } else {
                        *dest++ = RGB1(c);
                        break;
                    }
                }
                case 2:
                    *dest++ = RGBA(src[0], src[0], src[0], src[1]);
                    break;
                case 3: {
                    unsigned char r = src[0];
                    unsigned char g = src[1];
                    unsigned char b = src[2];
                    if (trns && trns[1] == r && trns[3] == g && trns[5] == b) {
                        *dest++ = RGBA(r, g, b, 0);
                        break;
                    } else {
                        *dest++ = RGB(r, g, b);
                        break;
                    }
                }
                case 4:
                    *dest++ = RGBA(src[0], src[1], src[2], src[3]);
                    break;
            }
        }
    }
}

static void depalette(int w, int h, unsigned char* src, int* dest, int bipp, const unsigned char* plte, const unsigned char* trns, int trnsSize) {
    int x, y, c;
    unsigned char alpha;
    int mask, len;

    switch (bipp) {
        case 4:
            mask = 15;
            len = 1;
            break;
        case 2:
            mask = 3;
            len = 3;
            break;
        case 1:
            mask = 1;
            len = 7;
    }

    for (y = 0; y < h; y++) {
        src++;  // skip filter byte
        for (x = 0; x < w; x++) {
            if (bipp == 8) {
                c = *src++;
            } else {
                int pos = x & len;
                c = (src[0] >> ((len - pos) * bipp)) & mask;
                if (pos == len) {
                    src++;
                }
            }
            alpha = 255;
            if (c < trnsSize) {
                alpha = trns[c];
            }
            *dest++ = RGBA(plte[c * 3 + 0], plte[c * 3 + 1], plte[c * 3 + 2], alpha);
        }
    }
}

static int outsize(Image* bmp, int bipp) {
    return (rowBytes(bmp->w, bipp) + 1) * bmp->h;
}

#define PNG_FAIL()      \
    {                   \
        errno = EINVAL; \
        goto err;       \
    }
#define PNG_CHECK(X) \
    if (!(X))        \
        PNG_FAIL()

typedef struct {
    unsigned bits, count;
    const unsigned char *in, *inend;
    unsigned char *out, *outend;
    jmp_buf jmp;
    unsigned litcodes[288], distcodes[32], lencodes[19];
    int tlit, tdist, tlen;
} State;

#define INFLATE_FAIL() longjmp(s->jmp, 1)
#define INFLATE_CHECK(X) \
    if (!(X))            \
        INFLATE_FAIL()

// Built-in DEFLATE standard tables.
static char order[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
static char lenBits[29 + 2] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
                                3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0 };
static int lenBase[29 + 2] = { 3,  4,  5,  6,  7,  8,  9,  10,  11,  13,  15,  17,  19,  23, 27, 31,
                               35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0,  0 };
static char distBits[30 + 2] = { 0, 0, 0, 0, 1, 1, 2,  2,  3,  3,  4,  4,  5,  5,  6, 6,
                                 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 0, 0 };
static int distBase[30 + 2] = {
    1,   2,   3,   4,   5,   7,    9,    13,   17,   25,   33,   49,   65,    97,    129,
    193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
};

static const unsigned char reverseTable[256] = {
#define R2(n) n, n + 128, n + 64, n + 192
#define R4(n) R2(n), R2(n + 32), R2(n + 16), R2(n + 48)
#define R6(n) R4(n), R4(n + 8), R4(n + 4), R4(n + 12)
    R6(0), R6(2), R6(1), R6(3)
};

static unsigned rev16(unsigned n) {
    return (reverseTable[n & 0xff] << 8) | reverseTable[(n >> 8) & 0xff];
}

static int bits(State* s, int n) {
    int v = s->bits & ((1 << n) - 1);
    s->bits >>= n;
    s->count -= n;
    while (s->count < 16) {
        INFLATE_CHECK(s->in != s->inend);
        s->bits |= (*s->in++) << s->count;
        s->count += 8;
    }
    return v;
}

static unsigned char* emit(State* s, int len) {
    s->out += len;
    INFLATE_CHECK(s->out <= s->outend);
    return s->out - len;
}

static void copy(State* s, const unsigned char* src, int len) {
    unsigned char* dest = emit(s, len);
    while (len--)
        *dest++ = *src++;
}

static int build(State* s, unsigned* tree, unsigned char* lens, int symcount) {
    int n, codes[16], first[16], counts[16] = { 0 };

    // Frequency count.
    for (n = 0; n < symcount; n++)
        counts[lens[n]]++;

    // Distribute codes.
    counts[0] = codes[0] = first[0] = 0;
    for (n = 1; n <= 15; n++) {
        codes[n] = (codes[n - 1] + counts[n - 1]) << 1;
        first[n] = first[n - 1] + counts[n - 1];
    }
    INFLATE_CHECK(first[15] + counts[15] <= symcount);

    // Insert keys into the tree for each symbol.
    for (n = 0; n < symcount; n++) {
        int len = lens[n];
        if (len != 0) {
            int code = codes[len]++, slot = first[len]++;
            tree[slot] = (code << (32 - len)) | (n << 4) | len;
        }
    }

    return first[15];
}

static int decode(State* s, unsigned tree[], int max) {
    // Find the next prefix code.
    unsigned lo = 0, hi = max, key;
    unsigned search = (rev16(s->bits) << 16) | 0xffff;
    while (lo < hi) {
        unsigned guess = (lo + hi) / 2;
        if (search < tree[guess])
            hi = guess;
        else
            lo = guess + 1;
    }

    // Pull out the key and check it.
    key = tree[lo - 1];
    INFLATE_CHECK(((search ^ key) >> (32 - (key & 0xf))) == 0);

    bits(s, key & 0xf);
    return (key >> 4) & 0xfff;
}

static void run(State* s, int sym) {
    int length = bits(s, lenBits[sym]) + lenBase[sym];
    int dsym = decode(s, s->distcodes, s->tdist);
    int offs = bits(s, distBits[dsym]) + distBase[dsym];
    copy(s, s->out - offs, length);
}

static void block(State* s) {
    for (;;) {
        int sym = decode(s, s->litcodes, s->tlit);
        if (sym < 256)
            *emit(s, 1) = (unsigned char)sym;
        else if (sym > 256)
            run(s, sym - 257);
        else
            break;
    }
}

static void stored(State* s) {
    // Uncompressed data block.
    int len;
    bits(s, s->count & 7);
    len = bits(s, 16);
    INFLATE_CHECK(((len ^ s->bits) & 0xffff) == 0xffff);
    INFLATE_CHECK(s->in + len <= s->inend);

    copy(s, s->in, len);
    s->in += len;
    bits(s, 16);
}

static void fixed(State* s) {
    // Fixed set of Huffman codes.
    int n;
    unsigned char lens[288 + 32];
    for (n = 0; n <= 143; n++)
        lens[n] = 8;
    for (n = 144; n <= 255; n++)
        lens[n] = 9;
    for (n = 256; n <= 279; n++)
        lens[n] = 7;
    for (n = 280; n <= 287; n++)
        lens[n] = 8;
    for (n = 0; n < 32; n++)
        lens[288 + n] = 5;

    // Build lit/dist trees.
    s->tlit = build(s, s->litcodes, lens, 288);
    s->tdist = build(s, s->distcodes, lens + 288, 32);
}

static void dynamic(State* s) {
    int n, i, nlit, ndist, nlen;
    unsigned char lenlens[19] = { 0 }, lens[288 + 32];
    nlit = 257 + bits(s, 5);
    ndist = 1 + bits(s, 5);
    nlen = 4 + bits(s, 4);
    for (n = 0; n < nlen; n++)
        lenlens[(unsigned char)order[n]] = (unsigned char)bits(s, 3);

    // Build the tree for decoding code lengths.
    s->tlen = build(s, s->lencodes, lenlens, 19);

    // Decode code lengths.
    for (n = 0; n < nlit + ndist;) {
        int sym = decode(s, s->lencodes, s->tlen);
        switch (sym) {
            case 16:
                for (i = 3 + bits(s, 2); i; i--, n++)
                    lens[n] = lens[n - 1];
                break;
            case 17:
                for (i = 3 + bits(s, 3); i; i--, n++)
                    lens[n] = 0;
                break;
            case 18:
                for (i = 11 + bits(s, 7); i; i--, n++)
                    lens[n] = 0;
                break;
            default:
                lens[n++] = (unsigned char)sym;
                break;
        }
    }

    // Build lit/dist trees.
    s->tlit = build(s, s->litcodes, lens, nlit);
    s->tdist = build(s, s->distcodes, lens + nlit, ndist);
}

static int inflate(void* out, unsigned outlen, const void* in, unsigned inlen) {
    int last;
    State* s = calloc(1, sizeof(State));

    // We assume we can buffer 2 extra bytes from off the end of 'in'.
    s->in = (unsigned char*)in;
    s->inend = s->in + inlen + 2;
    s->out = (unsigned char*)out;
    s->outend = s->out + outlen;
    s->bits = 0;
    s->count = 0;
    bits(s, 0);

    if (setjmp(s->jmp) == 1) {
        free(s);
        return 0;
    }

    do {
        last = bits(s, 1);
        switch (bits(s, 2)) {
            case 0:
                stored(s);
                break;
            case 1:
                fixed(s);
                block(s);
                break;
            case 2:
                dynamic(s);
                block(s);
                break;
            case 3:
                INFLATE_FAIL();
        }
    } while (!last);

    free(s);
    return 1;
}

static bool load_png(Image *img, PNG *png) {
    const unsigned char *ihdr, *idat, *plte, *trns, *first;
    int trnsSize = 0;
    int depth, ctype, bipp;
    int datalen = 0;
    unsigned char *data = NULL, *out;
    
    PNG_CHECK(memcmp(png->p, "\211PNG\r\n\032\n", 8) == 0);  // PNG signature
    png->p += 8;
    first = png->p;
    
    // Read IHDR
    ihdr = find(png, "IHDR", 13);
    PNG_CHECK(ihdr);
    depth = ihdr[8];
    ctype = ihdr[9];
    switch (ctype) {
        case 0:
            bipp = depth;
            break;  // greyscale
        case 2:
            bipp = 3 * depth;
            break;  // RGB
        case 3:
            bipp = depth;
            break;  // paletted
        case 4:
            bipp = 2 * depth;
            break;  // grey+alpha
        case 6:
            bipp = 4 * depth;
            break;  // RGBA
        default:
            PNG_FAIL();
    }
    
    // Allocate bitmap (+1 width to save room for stupid PNG filter bytes)
    InitImage(img, get32(ihdr + 0) + 1, get32(ihdr + 4));
    PNG_CHECK(img->buf);
    img->w--;
    
    // We support 8-bit color components and 1, 2, 4 and 8 bit palette formats.
    // No interlacing, or wacky filter types.
    PNG_CHECK((depth != 16) && ihdr[10] == 0 && ihdr[11] == 0 && ihdr[12] == 0);
    
    // Join IDAT chunks.
    for (idat = find(png, "IDAT", 0); idat; idat = find(png, "IDAT", 0)) {
        unsigned len = get32(idat - 8);
        data = realloc(data, datalen + len);
        if (!data)
            break;
        
        memcpy(data + datalen, idat, len);
        datalen += len;
    }
    
    // Find palette.
    png->p = first;
    plte = find(png, "PLTE", 0);
    
    // Find transparency info.
    png->p = first;
    trns = find(png, "tRNS", 0);
    if (trns) {
        trnsSize = get32(trns - 8);
    }
    
    PNG_CHECK(data && datalen >= 6);
    PNG_CHECK((data[0] & 0x0f) == 0x08  // compression method (RFC 1950)
          && (data[0] & 0xf0) <= 0x70   // window size
          && (data[1] & 0x20) == 0);    // preset dictionary present
    
    out = (unsigned char*)img->buf + outsize(img, 32) - outsize(img, bipp);
    PNG_CHECK(inflate(out, outsize(img, bipp), data + 2, datalen - 6));
    PNG_CHECK(unfilter(img->w, img->h, bipp, out));
    
    if (ctype == 3) {
        PNG_CHECK(plte);
        depalette(img->w, img->h, out, img->buf, bipp, plte, trns, trnsSize);
    } else {
        PNG_CHECK(bipp % 8 == 0);
        convert(bipp / 8, img->w, img->h, out, img->buf, trns);
    }
    
    free(data);
    return true;
    
err:
    if (data)
        free(data);
    if (img && img->buf)
        DestroyImage(img);
    return false;
}

static unsigned char* read_file(const char *path, size_t *sizeOfFile) {
    FILE *fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    unsigned char *data = malloc(size + 1);
    fread(data, size, 1, fp);
    fclose(fp);
    
    data[size] = '\0';
    *sizeOfFile = size;
    return data;
}

bool LoadImage(Image *out, const char *path) {
    unsigned char *data = NULL;
    size_t sizeOfData = 0;
    if (!(data = read_file(path, &sizeOfData)) && sizeOfData > 0)
        return false;
    bool result = LoadImageMemory(out, (void*)data, sizeOfData);
    free(data);
    return result;
}

bool LoadImageMemory(Image *out, const void *data, size_t sizeOfData) {
    PNG png = {
        .p = (unsigned char*)data,
        .end = (unsigned char*)data + sizeOfData
    };
    return load_png(out, &png);
}

typedef struct {
    unsigned crc, adler, bits, prev, runlen;
    FILE* out;
    unsigned crcTable[256];
} Save;

static const unsigned crctable[16] = { 0,          0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4,
                                       0x4db26158, 0x5005713c, 0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
                                       0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };

static void put(Save* s, unsigned v) {
    fputc(v, s->out);
    s->crc = (s->crc >> 4) ^ crctable[(s->crc & 15) ^ (v & 15)];
    s->crc = (s->crc >> 4) ^ crctable[(s->crc & 15) ^ (v >> 4)];
}

static void updateAdler(Save* s, unsigned v) {
    unsigned s1 = s->adler & 0xffff, s2 = (s->adler >> 16) & 0xffff;
    s1 = (s1 + v) % 65521;
    s2 = (s2 + s1) % 65521;
    s->adler = (s2 << 16) + s1;
}

static void put32(Save* s, unsigned v) {
    put(s, (v >> 24) & 0xff);
    put(s, (v >> 16) & 0xff);
    put(s, (v >> 8) & 0xff);
    put(s, v & 0xff);
}

void putbits(Save* s, unsigned data, unsigned bitcount) {
    while (bitcount--) {
        unsigned prev = s->bits;
        s->bits = (s->bits >> 1) | ((data & 1) << 7);
        data >>= 1;
        if (prev & 1) {
            put(s, s->bits);
            s->bits = 0x80;
        }
    }
}

void putbitsr(Save* s, unsigned data, unsigned bitcount) {
    while (bitcount--)
        putbits(s, data >> bitcount, 1);
}

static void begin(Save* s, const char* id, unsigned len) {
    put32(s, len);
    s->crc = 0xffffffff;
    put(s, id[0]);
    put(s, id[1]);
    put(s, id[2]);
    put(s, id[3]);
}

static void literal(Save* s, unsigned v) {
    // Encode a literal/length using the built-in tables.
    // Could do better with a custom table but whatever.
    if (v < 144)
        putbitsr(s, 0x030 + v - 0, 8);
    else if (v < 256)
        putbitsr(s, 0x190 + v - 144, 9);
    else if (v < 280)
        putbitsr(s, 0x000 + v - 256, 7);
    else
        putbitsr(s, 0x0c0 + v - 280, 8);
}

static void encodelen(Save* s, unsigned code, unsigned bits, unsigned len) {
    literal(s, code + (len >> bits));
    putbits(s, len, bits);
    putbits(s, 0, 5);
}

static void endrun(Save* s) {
    s->runlen--;
    literal(s, s->prev);

    if (s->runlen >= 67)
        encodelen(s, 277, 4, s->runlen - 67);
    else if (s->runlen >= 35)
        encodelen(s, 273, 3, s->runlen - 35);
    else if (s->runlen >= 19)
        encodelen(s, 269, 2, s->runlen - 19);
    else if (s->runlen >= 11)
        encodelen(s, 265, 1, s->runlen - 11);
    else if (s->runlen >= 3)
        encodelen(s, 257, 0, s->runlen - 3);
    else
        while (s->runlen--)
            literal(s, s->prev);
}

static void encodeByte(Save* s, unsigned char v) {
    updateAdler(s, v);

    // Simple RLE compression. We could do better by doing a search
    // to find matches, but this works pretty well TBH.
    if (s->prev == v && s->runlen < 115) {
        s->runlen++;
    } else {
        if (s->runlen)
            endrun(s);

        s->prev = v;
        s->runlen = 1;
    }
}

static void savePngHeader(Save* s, Image* bmp) {
    fwrite("\211PNG\r\n\032\n", 8, 1, s->out);
    begin(s, "IHDR", 13);
    put32(s, bmp->w);
    put32(s, bmp->h);
    put(s, 8);  // bit depth
    put(s, 6);  // RGBA
    put(s, 0);  // compression (deflate)
    put(s, 0);  // filter (standard)
    put(s, 0);  // interlace off
    put32(s, ~s->crc);
}

static long savePngData(Save* s, Image* bmp, long dataPos) {
    int x, y;
    long dataSize;
    begin(s, "IDAT", 0);
    put(s, 0x08);      // zlib compression method
    put(s, 0x1d);      // zlib compression flags
    putbits(s, 3, 3);  // zlib last block + fixed dictionary
    for (y = 0; y < bmp->h; y++) {
        int *row = &bmp->buf[y * bmp->w];
        int prev = RGBA1(0, 0);

        encodeByte(s, 1);  // sub filter
        for (x = 0; x < bmp->w; x++) {
            encodeByte(s, Rgba(row[x]) - Rgba(prev));
            encodeByte(s, rGba(row[x]) - rGba(prev));
            encodeByte(s, rgBa(row[x]) - rgBa(prev));
            encodeByte(s, rgbA(row[x]) - rgbA(prev));
            prev = row[x];
        }
    }
    endrun(s);
    literal(s, 256);  // terminator
    while (s->bits != 0x80)
        putbits(s, 0, 1);
    put32(s, s->adler);
    dataSize = (ftell(s->out) - dataPos) - 8;
    put32(s, ~s->crc);
    return dataSize;
}

bool SaveImage(Image *img, const char *path) {
    FILE* out = fopen(path, "wb");
    if (!out)
        return false;
    
    Save s;
    s.out = out;
    s.adler = 1;
    s.bits = 0x80;
    s.prev = 0xffff;
    s.runlen = 0;
    
    savePngHeader(&s, img);
    long dataPos = ftell(s.out);
    long dataSize = savePngData(&s, img, dataPos);
    
    // End chunk.
    begin(&s, "IEND", 0);
    put32(&s, ~s.crc);
    
    // Write back payload size.
    fseek(out, dataPos, SEEK_SET);
    put32(&s, dataSize);
    
    long err = ferror(out);
    fclose(out);
    return !err;
}

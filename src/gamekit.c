/* gamekit.c -- https://github.com/takeiteasy/c-gamekit

 The MIT License (MIT)
 Copyright (c) 2022 George Watson
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge,
 publish, distribute, sublicense, and/or sell copies of the Software,
 and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#define SOKOL_IMPL
#define JIM_IMPLEMENTATION
#define MJSON_IMPLEMENTATION
#define HASHMAP_IMPL
#define QOI_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define EZ_IMPLEMENTATION
#define IMAP_IMPLEMENTATION
#include "gamekit.h"

#if !defined(GAMEKIT_STATE)
static gkTexture* NewTexture(sg_image_desc *desc) {
    gkTexture *result = malloc(sizeof(gkTexture));
    result->internal = sg_make_image(desc);
    result->w = desc->width;
    result->h = desc->height;
    return result;
}

static gkTexture* EmptyTexture(unsigned int w, unsigned int h) {
    sg_image_desc desc = {
        .width = w,
        .height = h,
//        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .usage = SG_USAGE_STREAM
    };
    return NewTexture(&desc);
}

static void DestroyTexture(gkTexture *texture) {
    if (texture) {
        if (sg_query_image_state(texture->internal) == SG_RESOURCESTATE_VALID)
            sg_destroy_image(texture->internal);
        free(texture);
    }
}

#define QOI_MAGIC (((unsigned int)'q') << 24 | ((unsigned int)'o') << 16 | ((unsigned int)'i') <<  8 | ((unsigned int)'f'))

static bool CheckQOI(unsigned char *data) {
    return (data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3]) == QOI_MAGIC;
}

#define RGBA(R, G, B, A) (((unsigned int)(A) << 24) | ((unsigned int)(R) << 16) | ((unsigned int)(G) << 8) | (B))

static int* LoadImage(unsigned char *data, int sizeOfData, int *w, int *h) {
    assert(data && sizeOfData);
    int _w, _h, c;
    unsigned char *in = NULL;
    if (CheckQOI(data)) {
        qoi_desc desc;
        in = qoi_decode(data, sizeOfData, &desc, 4);
        _w = desc.width;
        _h = desc.height;
    } else
        in = stbi_load_from_memory(data, sizeOfData, &_w, &_h, &c, 4);
    assert(in && _w && _h);
    
    int *buf = malloc(_w * _h * sizeof(int));
    for (int x = 0; x < _w; x++)
        for (int y = 0; y < _h; y++) {
            unsigned char *p = in + (x + _w * y) * 4;
            buf[y * _w + x] = RGBA(p[0], p[1], p[2], p[3]);
        }
    free(in);
    if (w)
        *w = _w;
    if (h)
        *h = _h;
    return buf;
}

static void UpdateTexture(gkTexture *texture, int *data, int w, int h) {
    if (texture->w != w || texture->h != h) {
        DestroyTexture(texture);
        texture = EmptyTexture(w, h);
    }
    sg_image_data desc = {
        .subimage[0][0] = (sg_range) {
            .ptr = data,
            .size = w * h * sizeof(int)
        }
    };
    sg_update_image(texture->internal, &desc);
}

gkState state = {
    .running = false,
    .desc = (sapp_desc) {
#define X(NAME, TYPE, VAL, DEFAULT, DOCS) .VAL = DEFAULT,
        SETTINGS
#undef X
        .window_title = DEFAULT_WINDOW_TITLE
    },
    .pass_action = {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = {0}
        }
    }
};
#endif

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
//-----------------------------------------------------------------------------
static void MM86128(const void *key, const int len, uint32_t seed, void *out) {
#define ROTL32(x, r) ((x << r) | (x >> (32 - r)))
#define FMIX32(h) h^=h>>16; h*=0x85ebca6b; h^=h>>13; h*=0xc2b2ae35; h^=h>>16;
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 16;
    uint32_t h1 = seed;
    uint32_t h2 = seed;
    uint32_t h3 = seed;
    uint32_t h4 = seed;
    uint32_t c1 = 0x239b961b;
    uint32_t c2 = 0xab0e9789;
    uint32_t c3 = 0x38b34ae5;
    uint32_t c4 = 0xa1e38b93;
    const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);
    for (int i = -nblocks; i; i++) {
        uint32_t k1 = blocks[i*4+0];
        uint32_t k2 = blocks[i*4+1];
        uint32_t k3 = blocks[i*4+2];
        uint32_t k4 = blocks[i*4+3];
        k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
        h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;
        k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
        h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;
        k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
        h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;
        k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
        h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
    }
    const uint8_t * tail = (const uint8_t*)(data + nblocks*16);
    uint32_t k1 = 0;
    uint32_t k2 = 0;
    uint32_t k3 = 0;
    uint32_t k4 = 0;
    switch(len & 15) {
        case 15:
            k4 ^= tail[14] << 16;
        case 14:
            k4 ^= tail[13] << 8;
        case 13:
            k4 ^= tail[12] << 0;
            k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
        case 12:
            k3 ^= tail[11] << 24;
        case 11:
            k3 ^= tail[10] << 16;
        case 10:
            k3 ^= tail[ 9] << 8;
        case 9:
            k3 ^= tail[ 8] << 0;
            k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
        case 8:
            k2 ^= tail[ 7] << 24;
        case 7:
            k2 ^= tail[ 6] << 16;
        case 6:
            k2 ^= tail[ 5] << 8;
        case 5:
            k2 ^= tail[ 4] << 0;
            k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
        case 4:
            k1 ^= tail[ 3] << 24;
        case 3:
            k1 ^= tail[ 2] << 16;
        case 2:
            k1 ^= tail[ 1] << 8;
        case 1:
            k1 ^= tail[ 0] << 0;
            k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };
    h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;
    h1 += h2; h1 += h3; h1 += h4;
    h2 += h1; h3 += h1; h4 += h1;
    FMIX32(h1); FMIX32(h2); FMIX32(h3); FMIX32(h4);
    h1 += h2; h1 += h3; h1 += h4;
    h2 += h1; h3 += h1; h4 += h1;
    ((uint32_t*)out)[0] = h1;
    ((uint32_t*)out)[1] = h2;
    ((uint32_t*)out)[2] = h3;
    ((uint32_t*)out)[3] = h4;
}

static uint64_t MurmurHash(const void *data, size_t len, uint32_t seed) {
    char out[16];
    MM86128(data, (int)len, (uint32_t)seed, &out);
    return *(uint64_t*)out;
}

typedef struct {
    const char *name;
    gkInternalScene *wis;
} SceneBucket;

typedef enum {
    gkCommandProject,
    gkCommandResetProject,
    gkCommandPushTransform,
    gkCommandPopTransform,
    gkCommandResetTransform,
    gkCommandTranslate,
    gkCommandRotate,
    gkCommandRotateAt,
    gkCommandScale,
    gkCommandScaleAt,
    gkCommandResetPipeline,
    gkCommandSetUniform,
    gkCommandResetUniform,
    gkCommandSetBlendMode,
    gkCommandResetBlendMode,
    gkCommandSetColor,
    gkCommandResetColor,
    gkCommandUnsetImage,
    gkCommandResetImage,
    gkCommandResetSampler,
    gkCommandViewport,
    gkCommandResetViewport,
    gkCommandScissor,
    gkCommandResetScissor,
    gkCommandResetState,
    gkCommandClear,
    gkCommandDrawPoints,
    gkCommandDrawPoint,
    gkCommandDrawLines,
    gkCommandDrawLine,
    gkCommandDrawLinesStrip,
    gkCommandDrawFilledTriangles,
    gkCommandDrawFilledTriangle,
    gkCommandDrawFilledTrianglesStrip,
    gkCommandDrawFilledRects,
    gkCommandDrawFilledRect,
    gkCommandDrawTexturedRects,
    gkCommandDrawTexturedRect,
} gkCommandType;

typedef struct {
    gkCommandType type;
    void* data;
} gkCommand;

static void PushCommand(gkState* state, gkCommand* command) {
    ezStackAppend(&state->commandQueue, command->type, (void*)command);
}

typedef struct {
    float left;
    float right;
    float top;
    float bottom;
} gkProjectData;

void gkProject(gkState *state, float left, float right, float top, float bottom) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandProject;
    gkProjectData* cmdData = malloc(sizeof(gkProjectData));
    cmdData->left = left;
    cmdData->right = right;
    cmdData->top = top;
    cmdData->bottom = bottom;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

void gkResetProject(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetProject;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

void gkPushTransform(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandPushTransform;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

void gkPopTransform(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandPopTransform;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

void gkResetTransform(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetTransform;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

typedef struct {
    float x;
    float y;
} gkTranslateData;

void gkTranslate(gkState *state, float x, float y) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandTranslate;
    gkTranslateData* cmdData = malloc(sizeof(gkTranslateData));
    cmdData->x = x;
    cmdData->y = y;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    float theta;
} gkRotateData;

void gkRotate(gkState *state, float theta) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandRotate;
    gkRotateData* cmdData = malloc(sizeof(gkRotateData));
    cmdData->theta = theta;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    float theta;
    float x;
    float y;
} gkRotateAtData;

void gkRotateAt(gkState *state, float theta, float x, float y) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandRotateAt;
    gkRotateAtData* cmdData = malloc(sizeof(gkRotateAtData));
    cmdData->theta = theta;
    cmdData->x = x;
    cmdData->y = y;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    float sx;
    float sy;
} gkScaleData;

void gkScale(gkState *state, float sx, float sy) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandScale;
    gkScaleData* cmdData = malloc(sizeof(gkScaleData));
    cmdData->sx = sx;
    cmdData->sy = sy;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    float sx;
    float sy;
    float x;
    float y;
} gkScaleAtData;

void gkScaleAt(gkState *state, float sx, float sy, float x, float y) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandScaleAt;
    gkScaleAtData* cmdData = malloc(sizeof(gkScaleAtData));
    cmdData->sx = sx;
    cmdData->sy = sy;
    cmdData->x = x;
    cmdData->y = y;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

void gkResetPipeline(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetPipeline;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

typedef struct {
    void* data;
    int size;
} gkSetUniformData;

void gkSetUniform(gkState *state, void* data, int size) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandSetUniform;
    gkSetUniformData* cmdData = malloc(sizeof(gkSetUniformData));
    cmdData->data = data;
    cmdData->size = size;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

void gkResetUniform(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetUniform;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

typedef struct {
    sgp_blend_mode blend_mode;
} gkSetBlendModeData;

void gkSetBlendMode(gkState *state, sgp_blend_mode blend_mode) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandSetBlendMode;
    gkSetBlendModeData* cmdData = malloc(sizeof(gkSetBlendModeData));
    cmdData->blend_mode = blend_mode;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

void gkResetBlendMode(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetBlendMode;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

typedef struct {
    float r;
    float g;
    float b;
    float a;
} gkSetColorData;

void gkSetColor(gkState *state, float r, float g, float b, float a) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandSetColor;
    gkSetColorData* cmdData = malloc(sizeof(gkSetColorData));
    cmdData->r = r;
    cmdData->g = g;
    cmdData->b = b;
    cmdData->a = a;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

void gkResetColor(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetColor;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

typedef struct {
    int channel;
} gkUnsetImageData;

void gkUnsetImage(gkState *state, int channel) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandUnsetImage;
    gkUnsetImageData* cmdData = malloc(sizeof(gkUnsetImageData));
    cmdData->channel = channel;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    int channel;
} gkResetImageData;

void gkResetImage(gkState *state, int channel) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetImage;
    gkResetImageData* cmdData = malloc(sizeof(gkResetImageData));
    cmdData->channel = channel;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    int channel;
} gkResetSamplerData;

void gkResetSampler(gkState *state, int channel) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetSampler;
    gkResetSamplerData* cmdData = malloc(sizeof(gkResetSamplerData));
    cmdData->channel = channel;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    int x;
    int y;
    int w;
    int h;
} gkViewportData;

void gkViewport(gkState *state, int x, int y, int w, int h) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandViewport;
    gkViewportData* cmdData = malloc(sizeof(gkViewportData));
    cmdData->x = x;
    cmdData->y = y;
    cmdData->w = w;
    cmdData->h = h;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

void gkResetViewport(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetViewport;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

typedef struct {
    int x;
    int y;
    int w;
    int h;
} gkScissorData;

void gkScissor(gkState *state, int x, int y, int w, int h) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandScissor;
    gkScissorData* cmdData = malloc(sizeof(gkScissorData));
    cmdData->x = x;
    cmdData->y = y;
    cmdData->w = w;
    cmdData->h = h;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

void gkResetScissor(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetScissor;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

void gkResetState(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandResetState;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

void gkClear(gkState *state) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandClear;
    cmd->data = NULL;
    PushCommand(state, cmd);
}

typedef struct {
    sgp_point* points;
    int count;
} gkDrawPointsData;

void gkDrawPoints(gkState *state, sgp_point* points, int count) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawPoints;
    gkDrawPointsData* cmdData = malloc(sizeof(gkDrawPointsData));
    cmdData->points = points;
    cmdData->count = count;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    float x;
    float y;
} gkDrawPointData;

void gkDrawPoint(gkState *state, float x, float y) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawPoint;
    gkDrawPointData* cmdData = malloc(sizeof(gkDrawPointData));
    cmdData->x = x;
    cmdData->y = y;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    sgp_line* lines;
    int count;
} gkDrawLinesData;

void gkDrawLines(gkState *state, sgp_line* lines, int count) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawLines;
    gkDrawLinesData* cmdData = malloc(sizeof(gkDrawLinesData));
    cmdData->lines = lines;
    cmdData->count = count;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    float ax;
    float ay;
    float bx;
    float by;
} gkDrawLineData;

void gkDrawLine(gkState *state, float ax, float ay, float bx, float by) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawLine;
    gkDrawLineData* cmdData = malloc(sizeof(gkDrawLineData));
    cmdData->ax = ax;
    cmdData->ay = ay;
    cmdData->bx = bx;
    cmdData->by = by;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    sgp_point* points;
    int count;
} gkDrawLinesStripData;

void gkDrawLinesStrip(gkState *state, sgp_point* points, int count) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawLinesStrip;
    gkDrawLinesStripData* cmdData = malloc(sizeof(gkDrawLinesStripData));
    cmdData->points = points;
    cmdData->count = count;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    sgp_triangle* triangles;
    int count;
} gkDrawFilledTrianglesData;

void gkDrawFilledTriangles(gkState *state, sgp_triangle* triangles, int count) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawFilledTriangles;
    gkDrawFilledTrianglesData* cmdData = malloc(sizeof(gkDrawFilledTrianglesData));
    cmdData->triangles = triangles;
    cmdData->count = count;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    float ax;
    float ay;
    float bx;
    float by;
    float cx;
    float cy;
} gkDrawFilledTriangleData;

void gkDrawFilledTriangle(gkState *state, float ax, float ay, float bx, float by, float cx, float cy) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawFilledTriangle;
    gkDrawFilledTriangleData* cmdData = malloc(sizeof(gkDrawFilledTriangleData));
    cmdData->ax = ax;
    cmdData->ay = ay;
    cmdData->bx = bx;
    cmdData->by = by;
    cmdData->cx = cx;
    cmdData->cy = cy;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    sgp_point* points;
    int count;
} gkDrawFilledTrianglesStripData;

void gkDrawFilledTrianglesStrip(gkState *state, sgp_point* points, int count) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawFilledTrianglesStrip;
    gkDrawFilledTrianglesStripData* cmdData = malloc(sizeof(gkDrawFilledTrianglesStripData));
    cmdData->points = points;
    cmdData->count = count;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    sgp_rect* rects;
    int count;
} gkDrawFilledRectsData;

void gkDrawFilledRects(gkState *state, sgp_rect* rects, int count) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawFilledRects;
    gkDrawFilledRectsData* cmdData = malloc(sizeof(gkDrawFilledRectsData));
    cmdData->rects = rects;
    cmdData->count = count;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    float x;
    float y;
    float w;
    float h;
} gkDrawFilledRectData;

void gkDrawFilledRect(gkState *state, float x, float y, float w, float h) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawFilledRect;
    gkDrawFilledRectData* cmdData = malloc(sizeof(gkDrawFilledRectData));
    cmdData->x = x;
    cmdData->y = y;
    cmdData->w = w;
    cmdData->h = h;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    int channel;
    sgp_textured_rect* rects;
    int count;
} gkDrawTexturedRectsData;

void gkDrawTexturedRects(gkState *state, int channel, sgp_textured_rect* rects, int count) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawTexturedRects;
    gkDrawTexturedRectsData* cmdData = malloc(sizeof(gkDrawTexturedRectsData));
    cmdData->channel = channel;
    cmdData->rects = rects;
    cmdData->count = count;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

typedef struct {
    int channel;
    sgp_rect dest_rect;
    sgp_rect src_rect;
} gkDrawTexturedRectData;

void gkDrawTexturedRect(gkState *state, int channel, sgp_rect dest_rect, sgp_rect src_rect) {
    gkCommand* cmd = malloc(sizeof(gkCommand));
    cmd->type = gkCommandDrawTexturedRect;
    gkDrawTexturedRectData* cmdData = malloc(sizeof(gkDrawTexturedRectData));
    cmdData->channel = channel;
    cmdData->dest_rect = dest_rect;
    cmdData->src_rect = src_rect;
    cmd->data = cmdData;
    PushCommand(state, cmd);
}

static void FreeCommand(gkCommand* command) {
    gkCommandType type = command->type;
    switch
(type) {
    case gkCommandProject: {
        gkProjectData* data = (gkProjectData*)command->data;
        free(data);
        break;
    }
    case gkCommandTranslate: {
        gkTranslateData* data = (gkTranslateData*)command->data;
        free(data);
        break;
    }
    case gkCommandRotate: {
        gkRotateData* data = (gkRotateData*)command->data;
        free(data);
        break;
    }
    case gkCommandRotateAt: {
        gkRotateAtData* data = (gkRotateAtData*)command->data;
        free(data);
        break;
    }
    case gkCommandScale: {
        gkScaleData* data = (gkScaleData*)command->data;
        free(data);
        break;
    }
    case gkCommandScaleAt: {
        gkScaleAtData* data = (gkScaleAtData*)command->data;
        free(data);
        break;
    }
    case gkCommandSetUniform: {
        gkSetUniformData* data = (gkSetUniformData*)command->data;
        free(data);
        break;
    }
    case gkCommandSetBlendMode: {
        gkSetBlendModeData* data = (gkSetBlendModeData*)command->data;
        free(data);
        break;
    }
    case gkCommandSetColor: {
        gkSetColorData* data = (gkSetColorData*)command->data;
        free(data);
        break;
    }
    case gkCommandUnsetImage: {
        gkUnsetImageData* data = (gkUnsetImageData*)command->data;
        free(data);
        break;
    }
    case gkCommandResetImage: {
        gkResetImageData* data = (gkResetImageData*)command->data;
        free(data);
        break;
    }
    case gkCommandResetSampler: {
        gkResetSamplerData* data = (gkResetSamplerData*)command->data;
        free(data);
        break;
    }
    case gkCommandViewport: {
        gkViewportData* data = (gkViewportData*)command->data;
        free(data);
        break;
    }
    case gkCommandScissor: {
        gkScissorData* data = (gkScissorData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawPoints: {
        gkDrawPointsData* data = (gkDrawPointsData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawPoint: {
        gkDrawPointData* data = (gkDrawPointData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawLines: {
        gkDrawLinesData* data = (gkDrawLinesData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawLine: {
        gkDrawLineData* data = (gkDrawLineData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawLinesStrip: {
        gkDrawLinesStripData* data = (gkDrawLinesStripData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawFilledTriangles: {
        gkDrawFilledTrianglesData* data = (gkDrawFilledTrianglesData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawFilledTriangle: {
        gkDrawFilledTriangleData* data = (gkDrawFilledTriangleData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawFilledTrianglesStrip: {
        gkDrawFilledTrianglesStripData* data = (gkDrawFilledTrianglesStripData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawFilledRects: {
        gkDrawFilledRectsData* data = (gkDrawFilledRectsData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawFilledRect: {
        gkDrawFilledRectData* data = (gkDrawFilledRectData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawTexturedRects: {
        gkDrawTexturedRectsData* data = (gkDrawTexturedRectsData*)command->data;
        free(data);
        break;
    }
    case gkCommandDrawTexturedRect: {
        gkDrawTexturedRectData* data = (gkDrawTexturedRectData*)command->data;
        free(data);
        break;
    }
    default:
        break;
    }
    free(command);
}

static void ProcessCommand(gkCommand* command) {
    gkCommandType type = command->type;
    switch (type) {
    case gkCommandProject: {
        gkProjectData* data = (gkProjectData*)command->data;
        sgp_project(data->left, data->right, data->top, data->bottom);
        break;
    }
    case gkCommandResetProject:
        sgp_reset_project();
        break;
    case gkCommandPushTransform:
        sgp_push_transform();
        break;
    case gkCommandPopTransform:
        sgp_pop_transform();
        break;
    case gkCommandResetTransform:
        sgp_reset_transform();
        break;
    case gkCommandTranslate: {
        gkTranslateData* data = (gkTranslateData*)command->data;
        sgp_translate(data->x, data->y);
        break;
    }
    case gkCommandRotate: {
        gkRotateData* data = (gkRotateData*)command->data;
        sgp_rotate(data->theta);
        break;
    }
    case gkCommandRotateAt: {
        gkRotateAtData* data = (gkRotateAtData*)command->data;
        sgp_rotate_at(data->theta, data->x, data->y);
        break;
    }
    case gkCommandScale: {
        gkScaleData* data = (gkScaleData*)command->data;
        sgp_scale(data->sx, data->sy);
        break;
    }
    case gkCommandScaleAt: {
        gkScaleAtData* data = (gkScaleAtData*)command->data;
        sgp_scale_at(data->sx, data->sy, data->x, data->y);
        break;
    }
    case gkCommandResetPipeline:
        sgp_reset_pipeline();
        break;
    case gkCommandSetUniform: {
        gkSetUniformData* data = (gkSetUniformData*)command->data;
        sgp_set_uniform(data->data, data->size);
        break;
    }
    case gkCommandResetUniform:
        sgp_reset_uniform();
        break;
    case gkCommandSetBlendMode: {
        gkSetBlendModeData* data = (gkSetBlendModeData*)command->data;
        sgp_set_blend_mode(data->blend_mode);
        break;
    }
    case gkCommandResetBlendMode:
        sgp_reset_blend_mode();
        break;
    case gkCommandSetColor: {
        gkSetColorData* data = (gkSetColorData*)command->data;
        sgp_set_color(data->r, data->g, data->b, data->a);
        break;
    }
    case gkCommandResetColor:
        sgp_reset_color();
        break;
    case gkCommandUnsetImage: {
        gkUnsetImageData* data = (gkUnsetImageData*)command->data;
        sgp_unset_image(data->channel);
        break;
    }
    case gkCommandResetImage: {
        gkResetImageData* data = (gkResetImageData*)command->data;
        sgp_reset_image(data->channel);
        break;
    }
    case gkCommandResetSampler: {
        gkResetSamplerData* data = (gkResetSamplerData*)command->data;
        sgp_reset_sampler(data->channel);
        break;
    }
    case gkCommandViewport: {
        gkViewportData* data = (gkViewportData*)command->data;
        sgp_viewport(data->x, data->y, data->w, data->h);
        break;
    }
    case gkCommandResetViewport:
        sgp_reset_viewport();
        break;
    case gkCommandScissor: {
        gkScissorData* data = (gkScissorData*)command->data;
        sgp_scissor(data->x, data->y, data->w, data->h);
        break;
    }
    case gkCommandResetScissor:
        sgp_reset_scissor();
        break;
    case gkCommandResetState:
        sgp_reset_state();
        break;
    case gkCommandClear:
        sgp_clear();
        break;
    case gkCommandDrawPoints: {
        gkDrawPointsData* data = (gkDrawPointsData*)command->data;
        sgp_draw_points(data->points, data->count);
        break;
    }
    case gkCommandDrawPoint: {
        gkDrawPointData* data = (gkDrawPointData*)command->data;
        sgp_draw_point(data->x, data->y);
        break;
    }
    case gkCommandDrawLines: {
        gkDrawLinesData* data = (gkDrawLinesData*)command->data;
        sgp_draw_lines(data->lines, data->count);
        break;
    }
    case gkCommandDrawLine: {
        gkDrawLineData* data = (gkDrawLineData*)command->data;
        sgp_draw_line(data->ax, data->ay, data->bx, data->by);
        break;
    }
    case gkCommandDrawLinesStrip: {
        gkDrawLinesStripData* data = (gkDrawLinesStripData*)command->data;
        sgp_draw_lines_strip(data->points, data->count);
        break;
    }
    case gkCommandDrawFilledTriangles: {
        gkDrawFilledTrianglesData* data = (gkDrawFilledTrianglesData*)command->data;
        sgp_draw_filled_triangles(data->triangles, data->count);
        break;
    }
    case gkCommandDrawFilledTriangle: {
        gkDrawFilledTriangleData* data = (gkDrawFilledTriangleData*)command->data;
        sgp_draw_filled_triangle(data->ax, data->ay, data->bx, data->by, data->cx, data->cy);
        break;
    }
    case gkCommandDrawFilledTrianglesStrip: {
        gkDrawFilledTrianglesStripData* data = (gkDrawFilledTrianglesStripData*)command->data;
        sgp_draw_filled_triangles_strip(data->points, data->count);
        break;
    }
    case gkCommandDrawFilledRects: {
        gkDrawFilledRectsData* data = (gkDrawFilledRectsData*)command->data;
        sgp_draw_filled_rects(data->rects, data->count);
        break;
    }
    case gkCommandDrawFilledRect: {
        gkDrawFilledRectData* data = (gkDrawFilledRectData*)command->data;
        sgp_draw_filled_rect(data->x, data->y, data->w, data->h);
        break;
    }
    case gkCommandDrawTexturedRects: {
        gkDrawTexturedRectsData* data = (gkDrawTexturedRectsData*)command->data;
        sgp_draw_textured_rects(data->channel, data->rects, data->count);
        break;
    }
    case gkCommandDrawTexturedRect: {
        gkDrawTexturedRectData* data = (gkDrawTexturedRectData*)command->data;
        sgp_draw_textured_rect(data->channel, data->dest_rect, data->src_rect);
        break;
    }
    default:
        abort();
    }
}

#if defined(GAMEKIT_WINDOWS)
static FILETIME Win32GetLastWriteTime(char* path) {
    FILETIME time;
    WIN32_FILE_ATTRIBUTE_DATA data;

    if (GetFileAttributesEx(path, GetFileExInfoStandard, &data))
        time = data.ftLastWriteTime;

    return time;
}
#endif

#if !defined(GAMEKIT_STATE)
static bool ReloadLibrary(const char *path) {
#if defined(GAMEKIT_DISABLE_SCENE_RELOAD)
    return true;
#endif
    
#if defined(GAMEKIT_WINDOWS)
    FILETIME newTime = Win32GetLastWriteTime(path);
    bool result = CompareFileTime(&newTime, &state.libraryWriteTime);
    if (result)
        state.libraryWriteTime = newTime;
    else
        return true;
#else
    struct stat attr;
    bool result = !stat(path, &attr) && state.libraryHandleID != attr.st_ino;
    if (result)
        state.libraryHandleID = attr.st_ino;
    else
        return true;
#endif
    
    size_t libraryPathLength = state.libraryPath ? strlen(state.libraryPath) : 0;
    
    if (state.libraryHandle) {
        if (libraryPathLength != strlen(path) ||
            strncmp(state.libraryPath, path, libraryPathLength)) {
            if (state.libraryScene->deinit)
                state.libraryScene->deinit(&state, state.libraryContext);
        } else {
            if (state.libraryScene->unload)
                state.libraryScene->unload(&state, state.libraryContext);
        }
        dlclose(state.libraryHandle);
    }
    
#if defined(GAMEKIT_WINDOWS)
    size_t newPathSize = libraryPathLength + 4;
    char *newPath = malloc(sizeof(char) * newPathSize);
    char *noExt = RemoveExt(state.libraryPath);
    sprintf(newPath, "%s.tmp.dll", noExt);
    CopyFile(state.libraryPath, newPath, 0);
    if (!(state.libraryHandle = dlopen(newPath, RTLD_NOW)))
        goto BAIL;
    free(newPath);
    free(noExt);
    if (!state.libraryHandle)
#else
    if (!(state.libraryHandle = dlopen(path, RTLD_NOW)))
#endif
        goto BAIL;
    if (!(state.libraryScene = dlsym(state.libraryHandle, "scene")))
        goto BAIL;
    if (!state.libraryContext) {
        if (!(state.libraryContext = state.libraryScene->init(&state)))
            goto BAIL;
    } else {
        if (state.libraryScene->reload)
            state.libraryScene->reload(&state, state.libraryContext);
    }
    state.libraryPath = path;
    return true;

BAIL:
    if (state.libraryHandle)
        dlclose(state.libraryHandle);
    state.libraryHandle = NULL;
#if defined(GAMEKIT_WINDOWS)
    memset(&state.libraryWriteTime, 0, sizeof(FILETIME));
#else
    state.libraryHandleID = 0;
#endif
    return false;
}

static void Usage(const char *name) {
    printf("  usage: %s [options]\n\n  options:\n", name);
    printf("\t  help (flag) -- Show this message\n");
    printf("\t  config (string) -- Path to .json config file\n");
#define X(NAME, TYPE, VAL, DEFAULT, DOCS) \
    printf("\t  %s (%s) -- %s (default: %d)\n", NAME, #TYPE, DOCS, DEFAULT);
    SETTINGS
#undef X
}

static int LoadConfig(const char *path) {
    const char *data = NULL;
    if (!(data = LoadFile(path, NULL)))
        return 0;

    const struct json_attr_t config_attr[] = {
#define X(NAME, TYPE, VAL, DEFAULT,DOCS) \
        {(char*)#NAME, t_##TYPE, .addr.TYPE=&state.desc.VAL},
        SETTINGS
#undef X
        {NULL}
    };
    int status = json_read_object(data, config_attr, NULL);
    if (!status)
        return 0;
    free((void*)data);
    return 1;
}

#define jim_boolean jim_bool

static int ExportConfig(const char *path) {
    FILE *fh = fopen(path, "w");
    if (!fh)
        return 0;
    Jim jim = {
        .sink = fh,
        .write = (Jim_Write)fwrite
    };
    jim_object_begin(&jim);
#define X(NAME, TYPE, VAL, DEFAULT, DOCS) \
    jim_member_key(&jim, NAME);           \
    jim_##TYPE(&jim, state.desc.VAL);
    SETTINGS
#undef X
    jim_object_end(&jim);
    fclose(fh);
    return 1;
}

static int ParseArguments(int argc, char *argv[]) {
    const char *name = argv[0];
    sargs_desc desc = (sargs_desc) {
#if defined GAMEKIT_EMSCRIPTEN
        .argc = argc,
        .argv = (char**)argv
#else
        .argc = argc - 1,
        .argv = (char**)(argv + 1)
#endif
    };
    sargs_setup(&desc);
    
#if !defined(GAMEKIT_EMSCRIPTEN)
    if (sargs_exists("help")) {
        Usage(name);
        return 0;
    }
    if (sargs_exists("config")) {
        const char *path = sargs_value("config");
        if (!path) {
            fprintf(stderr, "[ARGUMENT ERROR] No value passed for \"config\"\n");
            Usage(name);
            return 0;
        }
        if (!DoesFileExist(path)) {
            fprintf(stderr, "[FILE ERROR] No file exists at \"%s\"\n", path);
            Usage(name);
            return 0;
        }
        LoadConfig(path);
    }
#endif // GAMEKIT_EMSCRIPTEN
    
#define boolean 1
#define integer 0
#define X(NAME, TYPE, VAL, DEFAULT, DOCS)                                               \
    if (sargs_exists(NAME))                                                             \
    {                                                                                   \
        const char *tmp = sargs_value_def(NAME, #DEFAULT);                              \
        if (!tmp)                                                                       \
        {                                                                               \
            fprintf(stderr, "[ARGUMENT ERROR] No value passed for \"%s\"\n", NAME);     \
            Usage(name);                                                                \
            return 0;                                                                   \
        }                                                                               \
        if (TYPE == 1)                                                                  \
            state.desc.VAL = (int)atoi(tmp);                                            \
        else                                                                            \
            state.desc.VAL = sargs_boolean(NAME);                                       \
    }
    SETTINGS
#undef X
#undef boolean
#undef integer
    return 1;
}

// MARK: Program loop

#define VALID_EXTS_LEN 9
static const char *validImages[VALID_EXTS_LEN] = {
    "jpg",
    "png",
    "tga",
    "bmp",
    "psd",
    "gdr",
    "pic",
    "pnm",
    "qoi"
};

static const char* ToLower(const char *str, int length) {
    if (!length)
        length = (int)strlen(str);
    assert(length);
    char *result = malloc(sizeof(char) * length);
    for (int i = 0; i < length; i++) {
        char c = str[i];
        result[i] = isalpha(c) && isupper(c) ? tolower(c) : c;
    }
    return result;
}

static void InitCallback(void) {
    sg_desc desc = (sg_desc) {
        // TODO: Add more configuration options for sg_desc
        .context = sapp_sgcontext()
    };
    sg_setup(&desc);
    stm_setup();
    sgp_desc desc_sgp = (sgp_desc) {};
    sgp_setup(&desc_sgp);
    assert(sg_isvalid() && sgp_is_valid());
    
    state.textureMapCapacity = 1;
    state.textureMapCount = 0;
    state.textureMap = imap_ensure(NULL, 1);
    state.assets = ezContainerRead(GAMEKIT_ASSETS_PATH);
    for (int i = 0; i < state.assets->sizeOfEntries; i++) {
        ezContainerTreeEntry *e = &state.assets->entries[i];
        const char *ext = FileExt(e->filePath);
        const char *extLower = ToLower(ext, 0);
        
        for (int i = 0; i < VALID_EXTS_LEN; i++)
            if (!strncmp(validImages[i], extLower, 3)) {
                if (++state.textureMapCount > state.textureMapCapacity) {
                    state.textureMapCapacity += state.textureMapCapacity;
                    state.textureMap = imap_ensure(state.textureMap, state.textureMapCapacity);
                }
                const char *fname = FileName(e->filePath);
                uint64_t hash = MurmurHash((void*)fname, strlen(fname), 0);
                imap_slot_t *slot = imap_assign(state.textureMap, hash);
                unsigned char *data = ezContainerEntryRaw(state.assets, &e->entry);
                int w, h;
                int *buf = LoadImage(data, (int)e->entry.fileSize, &w, &h);
                free(data);
                gkTexture *texture = EmptyTexture(w, h);
                UpdateTexture(texture, buf, w, h);
                free(buf);
                imap_setval64(state.textureMap, slot, (uint64_t)texture);
            }
        // TODO: Check other asset types
        free((void*)extLower);
    }
    
    state.windowWidth = sapp_width();
    state.windowHeight = sapp_height();
    state.clearColor = (sg_color){0.39f, 0.58f, 0.92f, 1.f};
    
#if defined(GAMEKIT_MAC)
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    uint64_t frequency = info.denom;
    frequency *= 1000000000L;
    state.timerFrequency = frequency / info.numer;
#elif defined(GAMEKIT_WINDOW)
    LARGE_INTEGER frequency;
    if (!QueryPerformanceFrequency(&frequency))
        return 1000L;
    state.timerFrequency = frequency.QuadPart;
#else
    state.timerFrequency = 1000000000L;
#endif
    
    state.updateMultiplicity = 1;
#if defined(GAMEKIT_UNLOCKFRAME_RATE)
    state.unlockFramerate = 1;
#else
    state.unlockFramerate = 0;
#endif
    state.desiredFrameTime = state.timerFrequency * DEFAULT_TARGET_FPS;
    state.fixedDeltaTime = 1.0 / DEFAULT_TARGET_FPS;
    int64_t time60hz = state.timerFrequency / 60;
    state.snapFrequencies[0] = time60hz;
    state.snapFrequencies[1] = time60hz*2;
    state.snapFrequencies[2] = time60hz*3;
    state.snapFrequencies[3] = time60hz*4;
    state.snapFrequencies[4] = (time60hz+1)/2;
    state.snapFrequencies[5] = (time60hz+2)/3;
    state.snapFrequencies[6] = (time60hz+3)/4;
    state.maxVsyncError = state.timerFrequency * .0002;
    for (int i = 0; i < 4; i++)
        state.timeAverager[i] = state.desiredFrameTime;
    state.resync = true;
    state.prevFrameTime = stm_now();
    state.frameAccumulator = 0;
    
#if defined(GAMEKIT_MAC)
#define DYLIB_EXT ".dylib"
#elif defined(GAMEKIT_WINDOWS)
#define DYLIB_EXT ".dll"
#elif defined(GAMEKIT_LINUX)
#define DYLIB_EXT ".so"
#else
#error Unsupported operating system
#endif
    
    // TODO: This needs reworking, temporary solution
#define LOAD_INITIAL_LIBRARY \
    assert(ReloadLibrary("./" GAMEKIT_DYLIB_PATH "/" GAMEKIT_FIRST_SCENE DYLIB_EXT));
    LOAD_INITIAL_LIBRARY
}

static void FrameCallback(void) {
    if (state.fullscreen != state.fullscreenLast) {
        sapp_toggle_fullscreen();
        state.fullscreenLast = state.fullscreen;
    }
    
    if (state.cursorVisible != state.cursorVisibleLast) {
        sapp_show_mouse(state.cursorVisible);
        state.cursorVisibleLast = state.cursorVisible;
    }
    
    if (state.cursorLocked != state.cursorLockedLast) {
        sapp_lock_mouse(state.cursorLocked);
        state.cursorLockedLast = state.cursorLocked;
    }
    
#if !defined(GAMEKIT_DISABLE_SCENE_RELOAD)
    assert(ReloadLibrary(state.libraryPath));
#endif
    
    if (state.libraryScene->preframe)
        state.libraryScene->preframe(&state, state.libraryContext);
    
    int64_t current_frame_time = stm_now();
    int64_t delta_time = current_frame_time - state.prevFrameTime;
    state.prevFrameTime = current_frame_time;
    
    if (delta_time > state.desiredFrameTime * 8)
        delta_time = state.desiredFrameTime;
    if (delta_time < 0)
        delta_time = 0;
    
    for (int i = 0; i < 7; ++i)
        if (labs(delta_time - state.snapFrequencies[i]) < state.maxVsyncError) {
            delta_time = state.snapFrequencies[i];
            break;
        }
    
    for (int i = 0; i < 3; ++i)
        state.timeAverager[i] = state.timeAverager[i + 1];
    state.timeAverager[3] = delta_time;
    delta_time = 0;
    for (int i = 0; i < 4; ++i)
        delta_time += state.timeAverager[i];
    delta_time /= 4.f;
    
    if ((state.frameAccumulator += delta_time) > state.desiredFrameTime * 8)
        state.resync = true;
    
    if (state.resync) {
        state.frameAccumulator = 0;
        delta_time = state.desiredFrameTime;
        state.resync = false;
    }
    
    double render_time = 1.0;
    if (state.unlockFramerate) {
        int64_t consumedDeltaTime = delta_time;
        
        while (state.frameAccumulator >= state.desiredFrameTime) {
            if (state.libraryScene->fixedupdate)
                state.libraryScene->fixedupdate(&state, state.libraryContext, state.fixedDeltaTime);
            if (consumedDeltaTime > state.desiredFrameTime) {
                if (state.libraryScene->update)
                    state.libraryScene->update(&state, state.libraryContext, state.fixedDeltaTime);
                consumedDeltaTime -= state.desiredFrameTime;
            }
            state.frameAccumulator -= state.desiredFrameTime;
        }
        
        if (state.libraryScene->update)
            state.libraryScene->update(&state, state.libraryContext, (double)consumedDeltaTime / state.timerFrequency);
        render_time = (double)state.frameAccumulator / state.desiredFrameTime;
    } else
        while (state.frameAccumulator >= state.desiredFrameTime*state.updateMultiplicity)
            for (int i = 0; i < state.updateMultiplicity; ++i) {
                if (state.libraryScene->fixedupdate)
                    state.libraryScene->fixedupdate(&state, state.libraryContext, state.fixedDeltaTime);
                if (state.libraryScene->update)
                    state.libraryScene->update(&state, state.libraryContext, state.fixedDeltaTime);
                state.frameAccumulator -= state.desiredFrameTime;
            }
    
    sgp_begin(state.windowWidth, state.windowHeight);
    sgp_set_color(state.clearColor.r,
                  state.clearColor.g,
                  state.clearColor.b,
                  state.clearColor.a);
    while (state.commandQueue.front) {
        gkCommand *command = (gkCommand*)state.commandQueue.front->data;
        ProcessCommand(command);
        FreeCommand(command);
        ezStackEntry *head = ezStackShift(&state.commandQueue);
        free(head);
    }
    sgp_clear();
    if (state.libraryScene->frame)
        state.libraryScene->frame(&state, state.libraryContext, render_time);
    // Assemble draw calls here
    
    sg_begin_default_pass(&state.pass_action, state.windowWidth, state.windowHeight);
    sgp_flush();
    sgp_end();
    sg_end_pass();
    sg_commit();
    
    if (state.libraryScene->postframe)
        state.libraryScene->postframe(&state, state.libraryContext);
}

static void EventCallback(const sapp_event* e) {
    switch (e->type) {
        case SAPP_EVENTTYPE_RESIZED:
            state.windowWidth = e->window_width;
            state.windowHeight = e->window_height;
            break;
        default:
            break;
    }
    if (state.libraryScene->event)
        state.libraryScene->event(&state, state.libraryContext, e);
}

static void CleanupCallback(void) {
    state.running = false;
    ezContainerFree(state.assets);
#define X(NAME)                                                                         \
    do {                                                                                \
        uint64_t hash = MurmurHash((void*)(NAME), strlen((NAME)), 0);                   \
        imap_slot_t *slot = imap_lookup(state.sceneMap, hash);                          \
        assert(slot);                                                                   \
        gkInternalScene *wis = (gkInternalScene*)imap_getval64(state.sceneMap, slot); \
        free(wis);                                                                      \
    } while (0);
//GAMEKIT_SCENES
#undef X
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
#if defined(GAMEKIT_ENABLE_CONFIG)
#if !defined(GAMEKIT_CONFIG_PATH)
    const char *configPath = JoinPath(UserPath(), DEFAULT_CONFIG_NAME);
#else
    const char *configPath = ResolvePath(GAMEKIT_CONFIG_PATH);
#endif
    
    if (DoesFileExist(configPath)) {
        if (!LoadConfig(configPath)) {
            fprintf(stderr, "[IMPORT CONFIG ERROR] Failed to import config from \"%s\"\n", configPath);
            fprintf(stderr, "errno (%d): \"%s\"\n", errno, strerror(errno));
            goto EXPORT_CONFIG;
        }
    } else {
    EXPORT_CONFIG:
        if (!ExportConfig(configPath)) {
            fprintf(stderr, "[EXPORT CONFIG ERROR] Failed to export config to \"%s\"\n", configPath);
            fprintf(stderr, "errno (%d): \"%s\"\n", errno, strerror(errno));
            abort();
        }
    }
#endif
#if defined(GAMEKIT_ENABLE_ARGUMENTS)
    if (argc > 1)
        if (!ParseArguments(argc, argv)) {
            fprintf(stderr, "[PARSE ARGUMENTS ERROR] Failed to parse arguments\n");
            abort();
        }
#endif
    
    state.desc.init_cb = InitCallback;
    state.desc.frame_cb = FrameCallback;
    state.desc.event_cb = EventCallback;
    state.desc.cleanup_cb = CleanupCallback;
    return state.desc;
}
#endif

void gkSetScene(gkState *state, const char *name) {
    
}

int gkWindowWidth(gkState *state) {
    return state->windowWidth;
}

int gkWindowHeight(gkState *state) {
    return state->windowHeight;
}

int gkIsWindowFullscreen(gkState *state) {
    return state->fullscreen;
}

void gkToggleFullscreen(gkState *state) {
    state->fullscreen = !state->fullscreen;
}

int gkIsCursorVisible(gkState *state) {
    return state->cursorVisible;
}

void gkToggleCursorVisible(gkState *state) {
    state->cursorVisible = !state->cursorVisible;
}

int gkIsCursorLocked(gkState *state) {
    return state->cursorLocked;
}

void gkToggleCursorLock(gkState *state) {
    state->cursorLocked = !state->cursorLocked;
}

uint64_t gkFindTexture(gkState *state, const char *name) {
    uint64_t hash = MurmurHash((void*)name, strlen(name), 0);
    return imap_lookup(state->textureMap, hash) ? hash : -1L;
}

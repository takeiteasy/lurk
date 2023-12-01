EXPORT void gkProject(gkState* state, float left, float right, float top, float bottom);
EXPORT void gkResetProject(gkState* state);
EXPORT void gkPushTransform(gkState* state);
EXPORT void gkPopTransform(gkState* state);
EXPORT void gkResetTransform(gkState* state);
EXPORT void gkTranslate(gkState* state, float x, float y);
EXPORT void gkRotate(gkState* state, float theta);
EXPORT void gkRotateAt(gkState* state, float theta, float x, float y);
EXPORT void gkScale(gkState* state, float sx, float sy);
EXPORT void gkScaleAt(gkState* state, float sx, float sy, float x, float y);
EXPORT void gkResetPipeline(gkState* state);
EXPORT void gkSetUniform(gkState* state, void* data, int size);
EXPORT void gkResetUniform(gkState* state);
EXPORT void gkSetBlendMode(gkState* state, sgp_blend_mode blend_mode);
EXPORT void gkResetBlendMode(gkState* state);
EXPORT void gkSetColor(gkState* state, float r, float g, float b, float a);
EXPORT void gkResetColor(gkState* state);
EXPORT void gkUnsetImage(gkState* state, int channel);
EXPORT void gkResetImage(gkState* state, int channel);
EXPORT void gkResetSampler(gkState* state, int channel);
EXPORT void gkViewport(gkState* state, int x, int y, int w, int h);
EXPORT void gkResetViewport(gkState* state);
EXPORT void gkScissor(gkState* state, int x, int y, int w, int h);
EXPORT void gkResetScissor(gkState* state);
EXPORT void gkResetState(gkState* state);
EXPORT void gkClear(gkState* state);
EXPORT void gkDrawPoints(gkState* state, sgp_point* points, int count);
EXPORT void gkDrawPoint(gkState* state, float x, float y);
EXPORT void gkDrawLines(gkState* state, sgp_line* lines, int count);
EXPORT void gkDrawLine(gkState* state, float ax, float ay, float bx, float by);
EXPORT void gkDrawLinesStrip(gkState* state, sgp_point* points, int count);
EXPORT void gkDrawFilledTriangles(gkState* state, sgp_triangle* triangles, int count);
EXPORT void gkDrawFilledTriangle(gkState* state, float ax, float ay, float bx, float by, float cx, float cy);
EXPORT void gkDrawFilledTrianglesStrip(gkState* state, sgp_point* points, int count);
EXPORT void gkDrawFilledRects(gkState* state, sgp_rect* rects, int count);
EXPORT void gkDrawFilledRect(gkState* state, float x, float y, float w, float h);
EXPORT void gkDrawTexturedRects(gkState* state, int channel, sgp_textured_rect* rects, int count);
EXPORT void gkDrawTexturedRect(gkState* state, int channel, sgp_rect dest_rect, sgp_rect src_rect);

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
    }
    case gkCommandRotate: {
        gkRotateData* data = (gkRotateData*)command->data;
        sgp_rotate(data->theta);
    }
    case gkCommandRotateAt: {
        gkRotateAtData* data = (gkRotateAtData*)command->data;
        sgp_rotate_at(data->theta, data->x, data->y);
    }
    case gkCommandScale: {
        gkScaleData* data = (gkScaleData*)command->data;
        sgp_scale(data->sx, data->sy);
    }
    case gkCommandScaleAt: {
        gkScaleAtData* data = (gkScaleAtData*)command->data;
        sgp_scale_at(data->sx, data->sy, data->x, data->y);
    }
    case gkCommandResetPipeline:
        sgp_reset_pipeline();
        break;
    case gkCommandSetUniform: {
        gkSetUniformData* data = (gkSetUniformData*)command->data;
        sgp_set_uniform(data->data, data->size);
    }
    case gkCommandResetUniform:
        sgp_reset_uniform();
        break;
    case gkCommandSetBlendMode: {
        gkSetBlendModeData* data = (gkSetBlendModeData*)command->data;
        sgp_set_blend_mode(data->blend_mode);
    }
    case gkCommandResetBlendMode:
        sgp_reset_blend_mode();
        break;
    case gkCommandSetColor: {
        gkSetColorData* data = (gkSetColorData*)command->data;
        sgp_set_color(data->r, data->g, data->b, data->a);
    }
    case gkCommandResetColor:
        sgp_reset_color();
        break;
    case gkCommandUnsetImage: {
        gkUnsetImageData* data = (gkUnsetImageData*)command->data;
        sgp_unset_image(data->channel);
    }
    case gkCommandResetImage: {
        gkResetImageData* data = (gkResetImageData*)command->data;
        sgp_reset_image(data->channel);
    }
    case gkCommandResetSampler: {
        gkResetSamplerData* data = (gkResetSamplerData*)command->data;
        sgp_reset_sampler(data->channel);
    }
    case gkCommandViewport: {
        gkViewportData* data = (gkViewportData*)command->data;
        sgp_viewport(data->x, data->y, data->w, data->h);
    }
    case gkCommandResetViewport:
        sgp_reset_viewport();
        break;
    case gkCommandScissor: {
        gkScissorData* data = (gkScissorData*)command->data;
        sgp_scissor(data->x, data->y, data->w, data->h);
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
    }
    case gkCommandDrawPoint: {
        gkDrawPointData* data = (gkDrawPointData*)command->data;
        sgp_draw_point(data->x, data->y);
    }
    case gkCommandDrawLines: {
        gkDrawLinesData* data = (gkDrawLinesData*)command->data;
        sgp_draw_lines(data->lines, data->count);
    }
    case gkCommandDrawLine: {
        gkDrawLineData* data = (gkDrawLineData*)command->data;
        sgp_draw_line(data->ax, data->ay, data->bx, data->by);
    }
    case gkCommandDrawLinesStrip: {
        gkDrawLinesStripData* data = (gkDrawLinesStripData*)command->data;
        sgp_draw_lines_strip(data->points, data->count);
    }
    case gkCommandDrawFilledTriangles: {
        gkDrawFilledTrianglesData* data = (gkDrawFilledTrianglesData*)command->data;
        sgp_draw_filled_triangles(data->triangles, data->count);
    }
    case gkCommandDrawFilledTriangle: {
        gkDrawFilledTriangleData* data = (gkDrawFilledTriangleData*)command->data;
        sgp_draw_filled_triangle(data->ax, data->ay, data->bx, data->by, data->cx, data->cy);
    }
    case gkCommandDrawFilledTrianglesStrip: {
        gkDrawFilledTrianglesStripData* data = (gkDrawFilledTrianglesStripData*)command->data;
        sgp_draw_filled_triangles_strip(data->points, data->count);
    }
    case gkCommandDrawFilledRects: {
        gkDrawFilledRectsData* data = (gkDrawFilledRectsData*)command->data;
        sgp_draw_filled_rects(data->rects, data->count);
    }
    case gkCommandDrawFilledRect: {
        gkDrawFilledRectData* data = (gkDrawFilledRectData*)command->data;
        sgp_draw_filled_rect(data->x, data->y, data->w, data->h);
    }
    case gkCommandDrawTexturedRects: {
        gkDrawTexturedRectsData* data = (gkDrawTexturedRectsData*)command->data;
        sgp_draw_textured_rects(data->channel, data->rects, data->count);
    }
    case gkCommandDrawTexturedRect: {
        gkDrawTexturedRectData* data = (gkDrawTexturedRectData*)command->data;
        sgp_draw_textured_rect(data->channel, data->dest_rect, data->src_rect);
    }
    default:
        abort();
    }
    FreeCommand(command);
}

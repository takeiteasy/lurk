/* gamekit.h -- https://github.com/takeiteasy/c-gamekit

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

#ifndef gamekit_h
#define gamekit_h
#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__EMSCRIPTEN__) || defined(EMSCRIPTEN)
#include <emscripten.h>
#define GAMEKIT_EMSCRIPTEN
#endif

#define GAMEKIT_POSIX
#if defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
#define GAMEKIT_MAC
#elif defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
#define GAMEKIT_WINDOWS
#if !defined(GAMEKIT_FORCE_POSIX)
#undef GAMEKIT_POSIX
#endif
#elif defined(__gnu_linux__) || defined(__linux__) || defined(__unix__)
#define GAMEKIT_LINUX
#else
#error "Unsupported operating system"
#endif

#if defined(PP_WINDOWS) && !defined(PP_NO_EXPORT)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#if !defined(__clang__) && (!defined(__GNUC__) || !defined(__GNUG__))
#error Unsupported compiler! This library relies on Clang/GCC extensions
#endif

#if defined(_MSC_VER) && _MSC_VER < 1800
#include <windef.h>
#define bool BOOL
#define true 1
#define false 0
#else
#if defined(__STDC__) && __STDC_VERSION__ < 199901L
typedef enum bool {
    false = 0,
    true = !false
} bool;
#else
#include <stdbool.h>
#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <sys/stat.h>
#include <dirent.h>
#include <setjmp.h>
#include <errno.h>
#include <assert.h>
#if defined(GAMEKIT_MAC)
#include <mach/mach_time.h>
#endif

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"
#if defined(GAMEKIT_STATE) && defined(SOKOL_IMPL)
#undef SOKOL_IMPL
#endif
#include "sokol_args.h"
#include "sokol_time.h"
#include "sokol_gp.h"
#include "jim.h"
#include "mjson.h"
#define IMAP_INTERFACE
#include "imap.h"
#include "qoi.h"
#define STB_NO_GIF
#include "stb_image.h"
#include "stb_image_write.h"
#include "ezimage.h"
#include "ezmath.h"
#include "ezrng.h"
#include "ezfs.h"
#include "ezcontainer.h"
#include "ezstack.h"

#include "config.h"

#if !defined(DEFAULT_CONFIG_NAME)
#if defined(GAMEKIT_POSIX)
#define DEFAULT_CONFIG_NAME ".gamekit.json"
#else
#define DEFAULT_CONFIG_NAME "gamekit.json"
#endif // GAMEKIT_POSX
#endif

#if !defined(DEFAULT_WINDOW_WIDTH)
#define DEFAULT_WINDOW_WIDTH 640
#endif

#if !defined(DEFAULT_WINDOW_HEIGHT)
#define DEFAULT_WINDOW_HEIGHT 480
#endif

#if !defined(DEFAULT_WINDOW_TITLE)
#define DEFAULT_WINDOW_TITLE "c-gamekit"
#endif

#if !defined(MAX_TEXTURE_STACK)
#define MAX_TEXTURE_STACK 8
#endif

#if defined(GAMEKIT_RELEASE)
#define GAMEKIT_DISABLE_SCENE_RELOAD
#endif

#if !defined(DEFAULT_TARGET_FPS)
#define DEFAULT_TARGET_FPS 60.f
#endif

#define SETTINGS                                                                                 \
    X("width", integer, width, DEFAULT_WINDOW_WIDTH, "Set window width")                         \
    X("height", integer, height, DEFAULT_WINDOW_HEIGHT, "Set window height")                     \
    X("sampleCount", integer, sample_count, 4, "Set the MSAA sample count of the   framebuffer") \
    X("swapInterval", integer, swap_interval, 1, "Set the preferred swap interval")              \
    X("highDPI", boolean, high_dpi, true, "Enable high-dpi compatability")                       \
    X("fullscreen", boolean, fullscreen, false, "Set fullscreen")                                \
    X("alpha", boolean, alpha, false, "Enable/disable alpha channel on framebuffers")            \
    X("clipboard", boolean, enable_clipboard, false, "Enable clipboard support")                 \
    X("clipboardSize", integer, clipboard_size, 1024, "Size of clipboard buffer (in bytes)")     \
    X("drapAndDrop", boolean, enable_dragndrop, false, "Enable drag-and-drop files")             \
    X("maxDroppedFiles", integer, max_dropped_files, 1, "Max number of dropped files")           \
    X("maxDroppedFilesPathLength", integer, max_dropped_file_path_length, MAX_PATH, "Max path length for dropped files")

typedef struct gkVertex {
    Vec2f position, texcoord;
    Vec4f color;
} gkVertex;

typedef struct gkRect {
    float x, y, w, h;
} gkRect;

typedef struct gkTexture {
    sg_image internal;
    int w, h;
} gkTexture;

typedef struct gkTextureBatch {
    gkTexture *texture;
    gkVertex *vertices;
    int maxVertices, vertexCount;
    sg_bindings bind;
    Vec2f size;
} gkTextureBatch;

typedef struct gkScene gkScene;
typedef struct GameKit GameKit;

typedef struct gkInternalScene {
    const char *path;
    void *handle;
#if defined(GAMEKIT_POSIX)
    ino_t handleID;
#else
    FILETIME writeTime;
#endif
    GameKit *context;
    gkScene *scene;
    struct gkInternalScene *next;
} gkInternalScene;

typedef struct gkState {
    gkInternalScene *currentScene;
    imap_node_t *sceneMap;
    imap_node_t *textureMap;
    int textureMapCapacity;
    int textureMapCount;
    ezContainer *assets;
    ezStack commandQueue;
    sg_color clearColor;

    uint64_t timerFrequency;
    int64_t prevFrameTime;
    int64_t desiredFrameTime;
    int64_t snapFrequencies[7];
    int64_t timeAverager[4];
    int64_t maxVsyncError;
    int64_t frameAccumulator;
    double fixedDeltaTime;
    bool resync;
    bool unlockFramerate;
    int updateMultiplicity;
    
    bool running;
    bool mouseHidden;
    bool mouseLocked;
    bool fullscreen, fullscreenLast;
    bool cursorVisible, cursorVisibleLast;
    bool cursorLocked, cursorLockedLast;
    int windowWidth, windowHeight;
    sapp_desc desc;

    sg_pass_action pass_action;
} gkState;

struct gkScene {
    GameKit*(*init)(gkState*);
    void (*deinit)(gkState*, GameKit*);
    void (*reload)(gkState*, GameKit*);
    void (*unload)(gkState*, GameKit*);
    void (*event)(gkState*, GameKit*, const sapp_event*);
    void (*preframe)(gkState*, GameKit*);
    bool (*update)(gkState*, GameKit*, float);
    bool (*fixedupdate)(gkState*, GameKit*, float);
    void (*frame)(gkState*, GameKit*, float);
    void (*postframe)(gkState*, GameKit*);
};

EXPORT void gkPushScene(gkState *state, const char *name);
EXPORT void gkPopScene(gkState *state);

EXPORT int gkWindowWidth(gkState *state);
EXPORT int gkWindowHeight(gkState *state);
EXPORT int gkIsWindowFullscreen(gkState *state);
EXPORT void gkToggleFullscreen(gkState *state);
EXPORT int gkIsCursorVisible(gkState *state);
EXPORT void gkToggleCursorVisible(gkState *state);
EXPORT int gkIsCursorLocked(gkState *state);
EXPORT void gkToggleCursorLock(gkState *state);

EXPORT uint64_t gkFindTexture(gkState *state, const char *name);
EXPORT uint64_t gkCreateTexture(gkState *state, int w, int h);
EXPORT void gkPushTexture(gkState *state, uint64_t tid);
EXPORT uint64_t gkPopTexture(gkState *state);
EXPORT void gkDrawTexture(gkState *state);

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

extern gkState state;

#if defined(__cplusplus)
}
#endif
#endif // gamekit_h

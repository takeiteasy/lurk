/* wee.h -- https://github.com/takeiteasy/wee

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

#ifndef wee_h
#define wee_h
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct wee wee;
#define weee wee
#define weeee wee
#define weeeee wee
#define weeeeee wee
#define weeeeeee wee
#define weeeeeeee wee
#define weeeeeeeee wee
#define weeeeeeeeee wee
#define weeeeeeeeeee wee
#define weeeeeeeeeeee wee
#define weeeeeeeeeeeee wee
#define weeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee

#if defined(__EMSCRIPTEN__) || defined(EMSCRIPTEN)
#include <emscripten.h>
#define WEE_EMSCRIPTEN
#endif

#define WEE_POSIX
#if defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
#define WEE_MAC
#elif defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
#define WEE_WINDOWS
#if !defined(WEE_FORCE_POSIX)
#undef WEE_POSIX
#endif
#elif defined(__gnu_linux__) || defined(__linux__) || defined(__unix__)
#define WEE_LINUX
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

#if defined(WEE_MAC)
#include <mach/mach_time.h>
#endif
#if defined(WEE_POSIX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <dlfcn.h>
#include <time.h>
#else // Windows
#include <io.h>
#include <windows.h>
#include <dirent.h>
#define F_OK 0
#define access _access
#define getcwd _getcwd
#define chdir _chdir
#endif

#include "sokol_gfx.h"
#include "sokol_args.h"
#include "sokol_app.h"
#include "sokol_glue.h"
#include "sokol_time.h"
#include "jim.h"
#include "mjson.h"
#include "hashmap.h"
#include "qoi.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "ezimage.h"
#include "ezmath.h"
#include "ezrng.h"
#include "ezecs.h"

#if !defined(MAX_PATH)
#if defined(WEE_MAC)
#define MAX_PATH 255
#elif defined(WEE_WINDOWS)
#define MAX_PATH 256
#elif defined(WEE_LINUX)
#define MAX_PATH 4096
#endif
#endif

#if defined(WEE_POSIX)
#define PATH_SEPERATOR '/'
#else
#define PATH_SEPERATOR '\\'
#endif

#include "config.h"

#if !defined(DEFAULT_CONFIG_NAME)
#if defined(WEE_POSIX)
#define DEFAULT_CONFIG_NAME ".wee.json"
#else
#define DEFAULT_CONFIG_NAME "wee.json"
#endif // WEE_POSX
#endif

#if !defined(DEFAULT_WINDOW_WIDTH)
#define DEFAULT_WINDOW_WIDTH 640
#endif

#if !defined(DEFAULT_WINDOW_HEIGHT)
#define DEFAULT_WINDOW_HEIGHT 480
#endif

#if !defined(DEFAULT_WINDOW_TITLE)
#define DEFAULT_WINDOW_TITLE "weeeeeeeeeeeeeeee"
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

EXPORT bool DoesFileExist(const char *path);
EXPORT bool DoesDirExist(const char *path);
EXPORT char *FormatString(const char *fmt, ...);
EXPORT char *LoadFile(const char *path, size_t *length);
EXPORT const char *JoinPath(const char *a, const char *b);
EXPORT const char *UserPath(void);
EXPORT const char *CurrentDirectory(void);
EXPORT bool SetCurrentDirectory(const char *path);
EXPORT const char *ResolvePath(const char *path);

typedef union {
    struct {
        unsigned char a, b, g, r;
    };
    int value;
} Color;

typedef struct {
    sg_image sgi;
    int w, h;
} Texture;

EXPORT Texture* LoadTextureFromImage(ezImage *img);
EXPORT Texture* LoadTextureFromFile(const char *path);
EXPORT Texture* CreateEmptyTexture(unsigned int w, unsigned int h);
EXPORT void UpdateTexture(Texture *texture, ezImage *img);
EXPORT void DestroyTexture(Texture *texture);

typedef struct weeScene weeScene;

typedef struct wis {
    const char *path;
    void *handle;
#if defined(WEE_POSIX)
    ino_t handleID;
#else
    FILETIME writeTime;
#endif
    weeeeeeeeeeeeeeeeeeeeeeeeeeeee *context;
    weeScene *scene;
    struct wis *next;
} weeInternalScene;

// TODO: Turn into easily modifiable option
#if !defined(WEE_MAX_MATRIX_STACK)
#define WEE_MAX_MATRIX_STACK 32
#endif

typedef struct {
    weeInternalScene *wis;
    struct hashmap *map;

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
    sg_pipeline pip;
} weeState;

struct weeScene {
    weeeee*(*init)(weeState*);
    void (*deinit)(weeState*, weeeeeee*);
    void (*reload)(weeState*, weeeeeee*);
    void (*unload)(weeState*, weeeeeee*);
    void (*event)(weeState*, weeeeeeee*, const sapp_event*);
    void (*preframe)(weeState*, weeeee*);
    bool (*update)(weeState*, weeeeeee*, float);
    bool (*fixedupdate)(weeState*, wee*, float);
    void (*frame)(weeState*, weeeeeeee*, float);
    void (*postframe)(weeState*, weeee*);
};

EXPORT void weeInit(weeState *state);
EXPORT void weeCreateScene(weeState *state, const char *name, const char *path);
EXPORT void weePushScene(weeState *state, const char *name);
EXPORT void weePopScene(weeState *state);
EXPORT void weeDestroyScene(weeState *state, const char *name);

EXPORT int weeWindowWidth(weeState *state);
EXPORT int weeWindowHeight(weeState *state);
EXPORT int weeIsWindowFullscreen(weeState *state);
EXPORT void weeToggleFullscreen(weeState *state);
EXPORT int weeIsCursorVisible(weeState *state);
EXPORT void weeToggleCursorVisible(weeState *state);
EXPORT int weeIsCursorLocked(weeState *state);
EXPORT void weeToggleCursorLock(weeState *state);

extern weeState state;

#if defined(__cplusplus)
}
#endif
#endif // wee_h

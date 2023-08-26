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

#if defined(WEE_POSIX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <dlfcn.h>
#else // Windows
#include <io.h>
#include <Windows.h>
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

#define weak __block

#define defer_merge(a, b) a##b
#define defer_varname(a) defer_merge(defer_scopevar_, a)
#define defer __attribute__((unused)) __attribute__((__cleanup__(defer_cleanup))) void (^defer_varname(__COUNTER__))(void) = ^

#define dtor(destructor) __attribute__((__cleanup__(destructor)))

// Taken from `map-macro` -- https://github.com/swansontec/map-macro
#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))

#define MAP_END(...)
#define MAP_OUT
#define MAP_COMMA ,

#define MAP_GET_END2() 0, MAP_END
#define MAP_GET_END1(...) MAP_GET_END2
#define MAP_GET_END(...) MAP_GET_END1
#define MAP_NEXT0(test, next, ...) next MAP_OUT
#define MAP_NEXT1(test, next) MAP_NEXT0(test, next, 0)
#define MAP_NEXT(test, next) MAP_NEXT1(MAP_GET_END test, next)

#define MAP0(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP1)(f, peek, __VA_ARGS__)
#define MAP1(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP0)(f, peek, __VA_ARGS__)

#define MAP_LIST_NEXT1(test, next) MAP_NEXT0(test, MAP_COMMA next, 0)
#define MAP_LIST_NEXT(test, next) MAP_LIST_NEXT1(MAP_GET_END test, next)

#define MAP_LIST0(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST1)(f, peek, __VA_ARGS__)
#define MAP_LIST1(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST0)(f, peek, __VA_ARGS__)

#define MAP(f, ...) EVAL(MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define MAP_LIST(f, ...) EVAL(MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define MAP_LIST0_UD(f, userdata, x, peek, ...) f(x, userdata) MAP_LIST_NEXT(peek, MAP_LIST1_UD)(f, userdata, peek, __VA_ARGS__)
#define MAP_LIST1_UD(f, userdata, x, peek, ...) f(x, userdata) MAP_LIST_NEXT(peek, MAP_LIST0_UD)(f, userdata, peek, __VA_ARGS__)

#define MAP_LIST_UD(f, userdata, ...) EVAL(MAP_LIST1_UD(f, userdata, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

// Taken from: https://gist.github.com/61131/7a22ac46062ee292c2c8bd6d883d28de
#define N_ARGS(...) _NARG_(__VA_ARGS__, _RSEQ())
#define _NARG_(...) _SEQ(__VA_ARGS__)
#define _SEQ(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125, _126, _127, N, ...) N
#define _RSEQ() 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

//! MARK: Filesystem

EXPORT bool DoesFileExist(const char *path);
EXPORT bool DoesDirExist(const char *path);
EXPORT char *FormatString(const char *fmt, ...);
EXPORT char *LoadFile(const char *path, size_t *length);
EXPORT const char *JoinPath(const char *a, const char *b);
EXPORT const char *UserPath(void);
EXPORT const char *CurrentDirectory(void);
EXPORT bool SetCurrentDirectory(const char *path);
EXPORT const char *ResolvePath(const char *path);

//! MARK: Maths

#ifndef PI
#define PI 3.14159265358979323846f
#endif
#ifndef TWO_PI
#define TWO_PI 6.28318530717958647692f
#endif
#ifndef TAU
#define TAU TWO_PI
#endif
#ifndef HALF_PI
#define HALF_PI 0.636619772367581343075f
#endif
#ifndef PHI
#define PHI 1.61803398874989484820f
#endif
#ifndef INV_PHI
#define INV_PHI 0.61803398874989484820f
#endif
#ifndef EULER
#define EULER 2.71828182845904523536f
#endif
#ifndef EPSILON
#define EPSILON 0.000001f
#endif

#define Bytes(n) (n)
#define Kilobytes(n) (n << 10)
#define Megabytes(n) (n << 20)
#define Gigabytes(n) (((uint64_t)n) << 30)
#define Terabytes(n) (((uint64_t)n) << 40)

#define Thousand(n) ((n)*1000)
#define Million(n) ((n)*1000000)
#define Billion(n) ((n)*1000000000LL)

#define U8Max ((uint8_t)0xFF)
#define U8Min ((uint8_t)0)
#define U16Max ((uint16_t)0xFFFF)
#define U16Min ((uint16_t)0)
#define U32Max ((uint32_t)0xFFFFFFFF)
#define U32Min ((uint32_t)0)
#define U64Max ((uint64_t)0xFFFFFFFFFFFFFFFF)
#define U64Min ((uint64_t)0)
#define S8Max ((int8_t)0x7F)
#define S8Min ((int8_t)-1 - 0x7F)
#define S16Max ((int16_t)0x7FFF)
#define S16Min ((int16_t)-1 - 0x7FFF)
#define S32Max ((int132_t)0x7FFFFFFF)
#define S32Min ((int32_t)-1 - 0x7FFFFFFF)
#define S64Max ((int64_t)0x7FFFFFFFFFFFFFFF)
#define S64Min ((int64_t)-1 - 0x7FFFFFFFFFFFFFFF)

#define MATRIX_TYPES \
    X(2, 2)          \
    X(3, 3)          \
    X(4, 4)

#define __MATRIX_T(W, H) Mat##W##H
#define __MATRIX_D(W, H, F) Mat##W##H##F
#define X(W, H)                                                        \
    typedef float __MATRIX_T(W, H) __attribute__((matrix_type(W, H))); \
    EXPORT __MATRIX_T(W, H) __MATRIX_D(W, H, Identity)(void);          \
    EXPORT float __MATRIX_D(W, H, Trace)(__MATRIX_T(W, H));            \
    EXPORT __MATRIX_T(W, H) __MATRIX_D(W, H, Zero)(void);
MATRIX_TYPES
#undef X

#define VECTOR_TYPES \
    X(2)             \
    X(3)             \
    X(4)

#define __VEC_T(L) Vec##L##f
#define __VEC_D(L, F) Vec##L##F
#define X(L)                                                                           \
    typedef float __VEC_T(L) __attribute__((ext_vector_type(L)));                      \
    typedef __VEC_T(L) Vector##L;                                                      \
    typedef int Vec##L##i __attribute__((ext_vector_type(L)));                         \
    EXPORT __VEC_T(L) __VEC_D(L, Zero)(void);                                          \
    EXPORT __VEC_T(L) __VEC_D(L, New)(float x, ...);                                   \
    EXPORT void __VEC_D(L, Print)(__VEC_T(L) v);                                       \
    EXPORT float __VEC_D(L, Sum)(__VEC_T(L) v);                                        \
    EXPORT bool __VEC_D(L, Equals)(__VEC_T(L) a, __VEC_T(L) b);                        \
    EXPORT float __VEC_D(L, LengthSqr)(__VEC_T(L) v);                                  \
    EXPORT float __VEC_D(L, Length)(__VEC_T(L) v);                                     \
    EXPORT float __VEC_D(L, Dot)(__VEC_T(L) v1, __VEC_T(L) v2);                        \
    EXPORT __VEC_T(L) __VEC_D(L, Normalize)(__VEC_T(L) v);                             \
    EXPORT float __VEC_D(L, DistSqr)(__VEC_T(L) v1, __VEC_T(L) v2);                    \
    EXPORT float __VEC_D(L, Dist)(__VEC_T(L) v1, __VEC_T(L) v2);                       \
    EXPORT __VEC_T(L) __VEC_D(L, Clamp)(__VEC_T(L) v, __VEC_T(L) min, __VEC_T(L) max); \
    EXPORT __VEC_T(L) __VEC_D(L, Min)(__VEC_T(L) v, __VEC_T(L) min);                   \
    EXPORT __VEC_T(L) __VEC_D(L, Max)(__VEC_T(L) v, __VEC_T(L) max);                   \
    EXPORT __VEC_T(L) __VEC_D(L, Lerp)(__VEC_T(L) v1, __VEC_T(L) v2, float n);
VECTOR_TYPES
#undef X

typedef Vec4f Quaternion;
typedef Vec2i Position;
typedef Mat44 Matrix;

EXPORT float Vec2Angle(Vec2f v1, Vec2f v2);
EXPORT Vec2f Vec2Rotate(Vec2f v, float angle);
EXPORT Vec2f Vec2MoveTowards(Vec2f v, Vec2f target, float maxDistance);
EXPORT Vec2f Vec2Reflect(Vec2f v, Vec2f normal);

EXPORT Vec3f Vec3Reflect(Vec3f v, Vec3f normal);
EXPORT Vec3f Vec3Cross(Vec3f v1, Vec3f v2);
EXPORT Vec3f Vec3Perpendicular(Vec3f v);
EXPORT float Vec3Angle(Vec3f v1, Vec3f v2);
EXPORT Vec3f Vec3RotateByQuaternion(Vec3f v, Quaternion q);
EXPORT Vec3f Vec3RotateByAxisAngle(Vec3f v, Vec3f axis, float angle);
EXPORT Vec3f Vec3Refract(Vec3f v, Vec3f n, float r);
EXPORT Vec3f Vec3Transform(Vec3f v, Matrix mat);
EXPORT Vec3f Vec3Barycentre(Vec3f p, Vec3f a, Vec3f b, Vec3f c);
EXPORT Vec3f Vec3Unproject(Vec3f source, Matrix projection, Matrix view);

#define QuaternionZero(...) Vec4Zero(__VA_ARGS__)
#define QuaternionNew(...) Vec4New(__VA_ARGS__)
#define QuaternionPrint(...) Vec4Print(__VA_ARGS__)
#define QuaternionSum(...) Vec4Sum(__VA_ARGS__)
#define QuaternionLengthSqr(...) Vec4LengthSqr(__VA_ARGS__)
#define QuaternionLength(...) Vec4Length(__VA_ARGS__)
#define QuaternionDot(...) Vec4Dot(__VA_ARGS__)
#define QuaternionNormalize(...) Vec4Normalize(__VA_ARGS__)
#define QuaternionDistSqr(...) Vec4DistSqr(__VA_ARGS__)
#define QuaternionDist(...) Vec4Dist(__VA_ARGS__)
#define QuaternionClamp(...) Vec4Clamp(__VA_ARGS__)
#define QuaternionMin(...) Vec4Min(__VA_ARGS__)
#define QuaternionMax(...) Vec4Max(__VA_ARGS__)
#define QuaternionLerp(...) Vec4Lerp(__VA_ARGS__)

EXPORT Quaternion QuaternionIdentity(void);
EXPORT Quaternion QuaternionMultiply(Quaternion q1, Quaternion q2);
EXPORT Quaternion QuaternionInvert(Quaternion q);
EXPORT Quaternion QuaternionFromVec3ToVec3(Vec3f from, Vec3f to);
EXPORT Quaternion QuaternionFromMatrix(Matrix mat);
EXPORT Matrix QuaternionToMatrix(Quaternion q);
EXPORT Quaternion QuaternionFromAxisAngle(Vector3 axis, float angle);
EXPORT void QuaternionToAxisAngle(Quaternion q, Vector3 *outAxis, float *outAngle);
EXPORT Quaternion QuaternionFromEuler(float pitch, float yaw, float roll);
EXPORT Vector3 QuaternionToEuler(Quaternion q);
EXPORT Quaternion QuaternionTransform(Quaternion q, Matrix mat);
EXPORT bool QuaternionEquals(Quaternion p, Quaternion q);

EXPORT float MatrixDetermint(Matrix mat);
EXPORT Matrix MatrixInvert(Matrix mat);
EXPORT Matrix MatrixTranslation(Vec3f v);
EXPORT Matrix MatrixRotation(Vec3f axis, float angle);
EXPORT Matrix MatrixScaling(Vec3f scale);
EXPORT Matrix MatrixFrustum(float left, float right, float bottom, float top, float near, float far);
EXPORT Matrix MatrixPerspective(float fovY, float aspect, float nearPlane, float farPlane);
EXPORT Matrix MatrixOrtho(float left, float right, float bottom, float top, float nearPlane, float farPlane);

EXPORT bool FloatCmp(float a, float b);
EXPORT int Min(int a, int b);
EXPORT int Max(int a, int b);
EXPORT int Clamp(int n, int min, int max);
EXPORT float Degrees(float radians);
EXPORT float Radians(float degrees);

// Taken from `raylib` -- https://github.com/raysan5/raylib/blob/master/examples/others/reasings.h
// Linear Easing functions
EXPORT float EaseLinearNone(float t, float b, float c, float d);
EXPORT float EaseLinearIn(float t, float b, float c, float d);
EXPORT float EaseLinearOut(float t, float b, float c, float d);
EXPORT float EaseLinearInOut(float t, float b, float c, float d);
// Sine Easing functions
EXPORT float EaseSineIn(float t, float b, float c, float d);
EXPORT float EaseSineOut(float t, float b, float c, float d);
EXPORT float EaseSineInOut(float t, float b, float c, float d);
// Circular Easing functions
EXPORT float EaseCircIn(float t, float b, float c, float d);
EXPORT float EaseCircOut(float t, float b, float c, float d);
EXPORT float EaseCircInOut(float t, float b, float c, float d);
// Cubic Easing functions
EXPORT float EaseCubicIn(float t, float b, float c, float d);
EXPORT float EaseCubicOut(float t, float b, float c, float d);
EXPORT float EaseCubicInOut(float t, float b, float c, float d);
// Quadratic Easing functions
EXPORT float EaseQuadIn(float t, float b, float c, float d);
EXPORT float EaseQuadOut(float t, float b, float c, float d);
EXPORT float EaseQuadInOut(float t, float b, float c, float d);
// Exponential Easing functions
EXPORT float EaseExpoIn(float t, float b, float c, float d);
EXPORT float EaseExpoOut(float t, float b, float c, float d);
EXPORT float EaseExpoInOut(float t, float b, float c, float d);
// Back Easing functions
EXPORT float EaseBackIn(float t, float b, float c, float d);
EXPORT float EaseBackOut(float t, float b, float c, float d);
EXPORT float EaseBackInOut(float t, float b, float c, float d);
// Bounce Easing functions
EXPORT float EaseBounceOut(float t, float b, float c, float d);
EXPORT float EaseBounceIn(float t, float b, float c, float d);
EXPORT float EaseBounceInOut(float t, float b, float c, float d);
// Elastic Easing functions
EXPORT float EaseElasticIn(float t, float b, float c, float d);
EXPORT float EaseElasticOut(float t, float b, float c, float d);
EXPORT float EaseElasticInOut(float t, float b, float c, float d);

// MARK: Random

typedef struct {
    unsigned int seed;
    int p1, p2;
    unsigned int buffer[17];
} Random;

EXPORT void InitRandom(Random *r, unsigned int s);
EXPORT unsigned int RandomBits(Random *r);
EXPORT float RandomFloat(Random *r);
EXPORT double RandomDouble(Random *r);
EXPORT int RandomInt(Random *r, int max);
EXPORT float RandomFloatRange(Random *r, float min, float max);
EXPORT double RandomDoubleRange(Random *r, double min, double max);
EXPORT int RandomIntRange(Random *r, int min, int max);

//! MARK: ECS

typedef union {
    struct {
        uint32_t id;
        uint16_t version;
        uint8_t unused;
        uint8_t flag;
    } parts;
    uint64_t id;
} Entity;

#define ENTITY_ID(E) ((E).parts.id)
#define ENTITY_VERSION(E) ((E).parts.version)
#define ENTITY_IS_NIL(E) ((E).parts.id == EcsNil)
#define ENTITY_CMP(A, B) ((A).id == (B).id)

#define EcsNil 0xFFFFFFFFull
#define EcsNilEntity (Entity) { .id = EcsNil }

#define ECS_COMPONENT(WORLD, T) EcsNewComponent((WORLD), sizeof(T))
#define ECS_TAG(WORLD) EcsNewComponent((WORLD), 0)
#define ECS_QUERY(WORLD, CB, UD, ...) EcsQuery((WORLD), CB, UD, (Entity[]){__VA_ARGS__}, sizeof((Entity[]){__VA_ARGS__}) / sizeof(Entity));
#define ECS_FIELD(QUERY, T, IDX) (T *)EcsQueryField(QUERY, IDX)
#define ECS_SYSTEM(WORLD, CB, ...) EcsNewSystem((WORLD), CB, N_ARGS(__VA_ARGS__), __VA_ARGS__)
#define ECS_PREFAB(WORLD, ...) EcsNewPrefab((WORLD), N_ARGS(__VA_ARGS__), __VA_ARGS__)
#define ECS_CHILDREN(WORLD, PARENT, CB) (EcsRelations((WORLD), (PARENT), EcsChildof, (CB)))
#define ENTITY_ISA(E, TYPE) ((E).parts.flag == Ecs##TYPE##Type)

typedef struct {
    Entity *sparse;
    Entity *dense;
    size_t sizeOfSparse;
    size_t sizeOfDense;
} EcsSparse;

typedef struct {
    Entity componentId;
    void *data;
    size_t sizeOfData;
    size_t sizeOfComponent;
    EcsSparse *sparse;
} EcsStorage;

typedef struct {
    EcsStorage **storages;
    size_t sizeOfStorages;
    Entity *entities;
    size_t sizeOfEntities;
    uint32_t *recyclable;
    size_t sizeOfRecyclable;
    uint32_t nextAvailableId;
} EcsWorld;

typedef struct {
    void **componentData;
    Entity *componentIndex;
    size_t sizeOfComponentData;
    Entity entity;
    void *userdata;
} Query;

typedef void (*SystemCb)(Query *query);

typedef struct {
    Entity *components;
    size_t sizeOfComponents;
} Prefab;

typedef struct {
    SystemCb callback;
    Entity *components;
    size_t sizeOfComponents;
    bool enabled;
} System;

typedef struct {
    Entity object,
    relation;
} Relation;

typedef enum {
    EcsEntityType = 0,
    EcsComponentType = (1 << 0),
    EcsSystemType = (1 << 1),
    EcsPrefabType = (1 << 2),
    EcsRelationType = (1 << 3)
} EntityFlag;

// MARK: ECS Functions

EXPORT void EcsNewWorld(EcsWorld *world);
EXPORT void EcsDestroyWorld(EcsWorld *world);

EXPORT Entity EcsNewEntity(EcsWorld *world);
EXPORT Entity EcsNewComponent(EcsWorld *world, size_t sizeOfComponent);
EXPORT Entity EcsNewSystem(EcsWorld *world, SystemCb fn, size_t sizeOfComponents, ...);
EXPORT Entity EcsNewPrefab(EcsWorld *world, size_t sizeOfComponents, ...);
EXPORT void DestroyEntity(EcsWorld *world, Entity entity);

EXPORT bool EcsIsValid(EcsWorld *world, Entity entity);
EXPORT bool EcsHas(EcsWorld *world, Entity entity, Entity component);
EXPORT void EcsAttach(EcsWorld *world, Entity entity, Entity component);
EXPORT void EcsAssociate(EcsWorld *world, Entity entity, Entity object, Entity relation);
EXPORT void EcsDetach(EcsWorld *world, Entity entity, Entity component);
EXPORT void EcsDisassociate(EcsWorld *world, Entity entity);
EXPORT bool EcsHasRelation(EcsWorld *world, Entity entity, Entity object);
EXPORT bool EcsRelated(EcsWorld *world, Entity entity, Entity relation);
EXPORT void *EcsGet(EcsWorld *world, Entity entity, Entity component);
EXPORT void EcsSet(EcsWorld *world, Entity entity, Entity component, const void *data);
EXPORT void EcsRelations(EcsWorld *world, Entity parent, Entity relation, void *userdata, SystemCb cb);

EXPORT void EcsEnableSystem(EcsWorld *world, Entity system);
EXPORT void EcsDisableSystem(EcsWorld *world, Entity system);
EXPORT void EcsEnableTimer(EcsWorld *world, Entity timer);
EXPORT void EcsDisableTimer(EcsWorld *world, Entity timer);

EXPORT void EcsRunSystem(EcsWorld *world, Entity system);
EXPORT void EcsStep(EcsWorld *world);
EXPORT void EcsQuery(EcsWorld *world, SystemCb cb, void *userdata, Entity *components, size_t sizeOfComponents);
EXPORT void *EcsQueryField(Query *query, size_t index);

extern Entity EcsSystem;
extern Entity EcsPrefab;
extern Entity EcsRelation;
extern Entity EcsChildOf;
extern Entity EcsTimer;

//! MARK: Image

typedef union {
    struct {
        unsigned char a, b, g, r;
    };
    int value;
} Color;

EXPORT Color RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
EXPORT Color RGB(unsigned char r, unsigned char g, unsigned char b);
EXPORT Color RGBA1(unsigned char c, unsigned char a);
EXPORT Color RGB1(unsigned char c);

typedef struct {
    int *buf, w, h;
} Image;

EXPORT Image* CreateImage(unsigned int w, unsigned int h);
EXPORT void DestroyImage(Image *img);

EXPORT void FillImage(Image *img, Color col);
EXPORT void FloodImage(Image *img, int x, int y, Color col);
EXPORT void ClearImage(Image *img);
EXPORT void BSet(Image *img, int x, int y, Color col);
EXPORT void PSet(Image *img, int x, int y, Color col);
EXPORT Color PGet(Image *img, int x, int y);
EXPORT void PasteImage(Image *dst, Image *src, int x, int y);
EXPORT void PasteImageClip(Image *dst, Image *src, int x, int y, int rx, int ry, int rw, int rh);
EXPORT Image* CopyImage(Image *b);
EXPORT void PassthruImage(Image *img, int (*fn)(int x, int y, Color col));
EXPORT Image* ScaleImage(Image *src, int nw, int nh);
EXPORT Image* RotateImage(Image *src, float angle);
EXPORT void DrawLine(Image *img, int x0, int y0, int x1, int y1, Color col);
EXPORT void DrawCircle(Image *img, int xc, int yc, int r, Color col, bool fill);
EXPORT void DrawRect(Image *img, int x, int y, int w, int h, Color col, bool fill);
EXPORT void DrawTri(Image *img, int x0, int y0, int x1, int y1, int x2, int y2, Color col, bool fill);

EXPORT Image* LoadImage(const char *path);
EXPORT Image* LoadImageMemory(const void *data, size_t length);
EXPORT bool SaveImage(Image *img, const char *path);

typedef struct {
    sg_image sgi;
    int w, h;
} Texture;

EXPORT Texture* LoadTextureFromImage(Image *img);
EXPORT Texture* LoadTextureFromFile(const char *path);
EXPORT Texture* CreateEmptyTexture(unsigned int w, unsigned int h);
EXPORT void UpdateTexture(Texture *texture, Image *img);
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

typedef struct Entry {
    int id;
    void *data;
    struct Entry *next, *prev;
} StackEntry;

typedef struct {
    StackEntry *front, *back;
} Stack;

#if !defined(WEE_MAX_MATRIX_STACK)
#define WEE_MAX_MATRIX_STACK 32
#endif

typedef struct {
    weeInternalScene *wis;
    struct hashmap *map;

    bool running;
    sapp_desc desc;

    sg_pass_action pass_action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
    sg_image color, depth;
    
    Stack commandStack;
    Mat44 matrixStack[WEE_MAX_MATRIX_STACK];
    int matrixStackCounter;
    Mat44 projectionMatrix, modelViewMatrix, textureMatrix, *currentMatrix;
} weeState;

struct weeScene {
    weeeeeeeeee *(*init)(weeState *);
    void (*deinit)(weeState*, weeeeeeee*);
    void (*reload)(weeState*, weeeeeeee*);
    void (*background)(weeState*, weeee*);
    void (*unload)(weeState*, weeeeeeee*);
    void (*event)(weeState*, weeeeeeeee*, const sapp_event*);
    bool (*frame)(weeState*, weeeeeeeee*, float);
};

EXPORT void weeInit(weeState *state);
EXPORT void weeCreateScene(weeState *state, const char *name, const char *path);
EXPORT void weePushScene(weeState *state, const char *name);
EXPORT void weePopScene(weeState *state);
EXPORT void weeDestroyScene(weeState *state, const char *name);

EXPORT void weeClearColor(weeState *state, Color color);
EXPORT void weeViewport(weeState *state, int x, int y, int width, int height);
EXPORT void weeScissorRect(weeState *state, int x, int y, int width, int height);

typedef enum {
    MATRIX_MODE_MODELVIEW,
    MATRIX_MODE_PROJECTION,
    MATRIX_MODE_TEXTURE
} MatrixMode;

EXPORT void weeMatrixMode(weeState *state, MatrixMode mode);
EXPORT void weePushMatrix(weeState *state);
EXPORT void weePopMatrix(weeState *state);
EXPORT void weeLoadIdentity(weeState *state);
EXPORT void weeTranslateMatrix(weeState *state, float x, float y, float z);
EXPORT void weeRotateMatirx(weeState *state, float angle, float x, float y, float z);
EXPORT void weeScaleMatrix(weeState *state, float x, float y, float z);
EXPORT void weeMulMatrix(weeState *state, Mat44 mat);
EXPORT void weeLoadFrustum(weeState *state, double left, double right, double bottom, double top, double znear, double zfar);
EXPORT void weeLoadOrtho(weeState *state, double left, double right, double bottom, double top, double znear, double zfar);

EXPORT void weeBeginPass(weeState *state);
EXPORT void weeDraw(weeState *state, int baseElement, int elementCount, int instanceCount);
EXPORT void weeEndPass(weeState *state);
EXPORT void weeCommit(weeState *state);

extern weeState state;

#if defined(__cplusplus)
}
#endif
#endif // wee_h

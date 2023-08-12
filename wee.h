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

#define wee
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

#if !defined(__clang__) && (!defined(__GNUC__) || !defined(__GNUG__))
#error This library relies on Clang/GCC extensions! Unsupported compiler!
#endif

#if defined(_MSC_VER) && _MSC_VER < 1800
#include <windef.h>
#define bool BOOL
#define true 1
#define false 0
#else
#if defined(__STDC__) && __STDC_VERSION__ < 199901L
typedef enum bool { false = 0, true = !false } bool;
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

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"
#include "sokol_args.h"
#include "sokol_time.h"
#include "jim.h"
#include "mjson.h"

#if defined(WEE_POSIX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#define PATH_SEPERATOR "/"
#else // Windows
#include <io.h>
#define F_OK    0
#define access _access
#define PATH_SEPERATOR "\\"
#endif

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
#define PATH_SEPERATOR "/"
#else
#define PATH_SEPERATOR "\\"
#endif

#if !defined(DEFAULT_CONFIG_NAME)
#if defined(WEE_POSIX)
#define DEFAULT_CONFIG_NAME ".wee.json"
#else
#define DEFAULT_CONFIG_NAME "wee.json"
#endif // WEE_POSX
#endif // DEFAULT_CONFIG_NAME

#if !defined(DEFAULT_WINDOW_WIDTH)
#define DEFAULT_WINDOW_WIDTH 640
#endif

#if !defined(DEFAULT_WINDOW_HEIGHT)
#define DEFAULT_WINDOW_HEIGHT 480
#endif

#if !defined(DEFAULT_WINDOW_TITLE)
#define DEFAULT_WINDOW_TITLE "WEE"
#endif

#define SETTINGS                                                                             \
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

// Taken from `map-macro` -- https://github.com/swansontec/map-macro
#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...)  EVAL4(EVAL4(EVAL4(__VA_ARGS__)))

#define MAP_END(...)
#define MAP_OUT
#define MAP_COMMA ,

#define MAP_GET_END2() 0, MAP_END
#define MAP_GET_END1(...) MAP_GET_END2
#define MAP_GET_END(...) MAP_GET_END1
#define MAP_NEXT0(test, next, ...) next MAP_OUT
#define MAP_NEXT1(test, next) MAP_NEXT0(test, next, 0)
#define MAP_NEXT(test, next)  MAP_NEXT1(MAP_GET_END test, next)

#define MAP0(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP1)(f, peek, __VA_ARGS__)
#define MAP1(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP0)(f, peek, __VA_ARGS__)

#define MAP_LIST_NEXT1(test, next) MAP_NEXT0(test, MAP_COMMA next, 0)
#define MAP_LIST_NEXT(test, next)  MAP_LIST_NEXT1(MAP_GET_END test, next)

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
#define _SEQ(_1, _2, _3, _4, _5, _6, _7, _8, _9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63,_64,_65,_66,_67,_68,_69,_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_80,_81,_82,_83,_84,_85,_86,_87,_88,_89,_90,_91,_92,_93,_94,_95,_96,_97,_98,_99,_100,_101,_102,_103,_104,_105,_106,_107,_108,_109,_110,_111,_112,_113,_114,_115,_116,_117,_118,_119,_120,_121,_122,_123,_124,_125,_126,_127,N,...) N
#define _RSEQ() 127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

//! MARK: Filesystem

bool DoesFileExist(const char *path);
bool DoesDirExist(const char *path);
char* FormatString(const char *fmt, ...);
char* LoadFile(const char *path, size_t *length);
const char* JoinPath(const char *a, const char *b);

//! MARK: Maths

#ifndef PI
#define PI 3.14159265358979323846f
#endif
#ifndef TWO_PI
#define TWO_PI  6.28318530717958647692f
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

#define Bytes(n)      (n)
#define Kilobytes(n)  (n << 10)
#define Megabytes(n)  (n << 20)
#define Gigabytes(n)  (((uint64_t)n) << 30)
#define Terabytes(n)  (((uint64_t)n) << 40)

#define Thousand(n) ((n)*1000)
#define Million(n)  ((n)*1000000)
#define Billion(n)  ((n)*1000000000LL)

#define U8Max  ((uint8_t)0xFF)
#define U8Min  ((uint8_t)0)
#define U16Max ((uint16_t)0xFFFF)
#define U16Min ((uint16_t)0)
#define U32Max ((uint32_t)0xFFFFFFFF)
#define U32Min ((uint32_t)0)
#define U64Max ((uint64_t)0xFFFFFFFFFFFFFFFF)
#define U64Min ((uint64_t)0)
#define S8Max  ((int8_t)0x7F)
#define S8Min  ((int8_t)-1 - 0x7F)
#define S16Max ((int16_t)0x7FFF)
#define S16Min ((int16_t)-1 - 0x7FFF)
#define S32Max ((int132_t)0x7FFFFFFF)
#define S32Min ((int32_t)-1 - 0x7FFFFFFF)
#define S64Max ((int64_t)0x7FFFFFFFFFFFFFFF)
#define S64Min ((int64_t)-1 - 0x7FFFFFFFFFFFFFFF)

#define SWZL_MAP(V, VEC) VEC.V
#define SWIZZLE(V, ...) { MAP_LIST_UD(SWZL_MAP, V, __VA_ARGS__) }

#define __MATRIX_T(W, H) Mat##W##H
#define __MATRIX_D(W, H, F) Mat##W##H##F
#define MATRIX_TYPES \
    X(2, 2)          \
    X(3, 3)          \
    X(4, 4)
#define X(W, H)                                                        \
    typedef float __MATRIX_T(W, H) __attribute__((matrix_type(W, H))); \
    __MATRIX_T(W, H)                                                   \
    __MATRIX_D(W, H, Identity)(void);                                  \
    __MATRIX_T(W, H)                                                   \
    __MATRIX_D(W, H, Zero)(void);
MATRIX_TYPES
#undef X

typedef union {
    Mat22 mat;
    struct {
        //    0   1
        float m0, m2; // 0
        float m1, m3; // 1
    };
} Matrix2;

typedef union {
    Mat33 mat;
    struct {
        //    0   1   2
        float m0, m3, m6; // 0
        float m1, m4, m7; // 1
        float m2, m5, m8; // 2
    };
} Matrix3;

typedef union {
    Mat44 mat;
    struct {
        //    0   1   2    3
        float m0, m4, m8,  m12; // 0
        float m1, m5, m9,  m13; // 1
        float m2, m6, m10, m14; // 2
        float m3, m7, m11, m15; // 3
    };
} Matrix;

#define __VEC_T(L) Vec##L
#define __VEC_D(L, F) Vec##L##F
#define VECTOR_TYPES \
    X(2)             \
    X(3)             \
    X(4)
#define X(L)                                                         \
    typedef float __VEC_T(L) __attribute__((ext_vector_type(L)));    \
    __VEC_T(L)                                                       \
    __VEC_D(L, Zero)(void);                                          \
    __VEC_T(L)                                                       \
    __VEC_D(L, New)(float x, ...);                                   \
    void __VEC_D(L, Print)(__VEC_T(L) v);                            \
    float __VEC_D(L, Sum)(__VEC_T(L) v);                             \
    float __VEC_D(L, LengthSqr)(__VEC_T(L) v);                       \
    float __VEC_D(L, Length)(__VEC_T(L) v);                          \
    float __VEC_D(L, Dot)(__VEC_T(L) v1, __VEC_T(L) v2);             \
    __VEC_T(L)                                                       \
    __VEC_D(L, Normalize)(__VEC_T(L) v);                             \
    float __VEC_D(L, DistSqr)(__VEC_T(L) v1, __VEC_T(L) v2);         \
    float __VEC_D(L, Dist)(__VEC_T(L) v1, __VEC_T(L) v2);            \
    __VEC_T(L)                                                       \
    __VEC_D(L, Clamp)(__VEC_T(L) v, __VEC_T(L) min, __VEC_T(L) max); \
    __VEC_T(L)                                                       \
    __VEC_D(L, Min)(__VEC_T(L) v, __VEC_T(L) min);                   \
    __VEC_T(L)                                                       \
    __VEC_D(L, Max)(__VEC_T(L) v, __VEC_T(L) max);                   \
    __VEC_T(L)                                                       \
    __VEC_D(L, Lerp)(__VEC_T(L) v1, __VEC_T(L) v2, float n);
VECTOR_TYPES
#undef X

typedef union {
    struct {
        float x;
        float y;
    };
    float xy[2];
    struct {
        float u;
        float v;
    };
    float uv[2];
    struct {
        float s;
        float t;
    };
    float st[2];
    float elements[2];
    Vec2 vec;
} Vector2f;

typedef union {
    struct {
        float x;
        float y;
        float z;
    };
    float xyz[3];
    struct {
        Vector2f xy;
        float _;
    };
    struct {
        float __;
        Vector2f yz;
    };
    struct {
        float r;
        float g;
        float b;
    };
    float rgb[3];
    struct {
        Vector2f rg;
        float ___;
    };
    struct {
        float ____;
        Vector2f gb;
    };
    struct {
        float s;
        float t;
        float p;
    };
    float stp[3];
    struct {
        Vector2f st;
        float _____;
    };
    struct {
        float ______;
        Vector2f tp;
    };
    float elements[3];
    Vec3 vec;
} Vector3f;

typedef union {
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    float xyzw[4];
    struct {
        Vector2f xy;
        Vector2f zw;
    };
    struct {
        Vector3f xyz;
        float _;
    };
    struct {
        float __;
        Vector3f yzw;
    };
    struct {
        float r;
        float g;
        float b;
        float a;
    };
    float rgba[4];
    struct {
        Vector2f rg;
        Vector2f ba;
    };
    struct {
        Vector3f rgb;
        float ___;
    };
    struct {
        float ____;
        Vector3f gba;
    };
    struct {
        float s;
        float t;
        float p;
        float q;
    };
    float stpq[4];
    struct {
        Vector2f st;
        Vector2f pq;
    };
    struct {
        Vector3f stp;
        float _____;
    };
    struct {
        float ______;
        Vector3f tpq;
    };
    float elements[4];
    Vec4 vec;
} Vector4f;

typedef float VecNf[];
typedef int VecNi[];

typedef Vec4 Quat;
typedef Vector4f Quaternion;
#define X(L) typedef int Position##L __attribute__((ext_vector_type(L)));
VECTOR_TYPES
#undef X
typedef Position2 Position;
typedef Matrix Matrix4;

#define Vec(v) (v.vec)
#define Mat(m) (m.mat)
#define Vector2(v) ((Vector2f) { .vec = v })
#define Vector3(v) ((Vector3f) { .vec = v })
#define Vector4(v) ((Vector4f) { .vec = v })

#define X(N) Matrix##N Matrix##N##Identity(void);
VECTOR_TYPES
#undef X
#define MatrixIdentity Matrix4Identity

#define Vector2Angle(v1, v2) Vec2Angle(Vec(v1), Vec(v2))
float Vec2Angle(Vec2 v1, Vec2 v2);
#define Vector2Rotate(v, angle) Vector2(Vec2Rotate(Vec(v), angle))
Vec2 Vec2Rotate(Vec2 v, float angle);
#define Vector2MoveTowards(v, target, maxDistance) Vector2(Vec2MoveTowards(Vec(v), Vec(target), maxDistance))
Vec2 Vec2MoveTowards(Vec2 v, Vec2 target, float maxDistance);
#define Vector2Reflect(v, normal) Vector2(Vec2Reflect(Vec(v), Vec(normal)))
Vec2 Vec2Reflect(Vec2 v, Vec2 normal);

#define Vector3Reflect(v, normal) Vector3(Vec3Reflect(Vec(v), Vec(normal)))
Vec3 Vec3Reflect(Vec3 v, Vec3 normal);
#define Vector3Cross(v, v2) Vector3(Vec3Cross(Vec(v), Vec(v2)))
Vec3 Vec3Cross(Vec3 v1, Vec3 v2);
#define Vector3Perpendicular(v) Vector3(Vec3Perpendicular(Vec(v)))
Vec3 Vec3Perpendicular(Vec3 v);
#define Vector3Angle(v1, v2) Vec3Angle(Vec(v1), Vec(v2))
float Vec3Angle(Vec3 v1, Vec3 v2);
#define Vector3RotateByQuaternion(v, q) Vector3(Vec3RotateByQuaternion(Vec(v), Vec(q)))
Vec3 Vec3RotateByQuaternion(Vec3 v, Quat q);
#define Vector3RotateByAxisAngle(v, axis, angle) Vector3(Vec3RotateByAxisAngle(Vec(v), Vec(axis), angle))
Vec3 Vec3RotateByAxisAngle(Vec3 v, Vec3 axis, float angle);
#define Vector3Refract(v, n, r) Vector3(Vec3Refract(Vec(v), Vec(n), r))
Vec3 Vec3Refract(Vec3 v, Vec3 n, float r);

int FloatCmp(float a, float b);
int Min(int a, int b);
int Max(int a, int b);
int Clamp(int n, int min, int max);
float Degrees(float radians);
float Radians(float degrees);

// Taken from `raylib` -- https://github.com/raysan5/raylib/blob/master/examples/others/reasings.h
// Linear Easing functions
float EaseLinearNone(float t, float b, float c, float d);
float EaseLinearIn(float t, float b, float c, float d);
float EaseLinearOut(float t, float b, float c, float d);
float EaseLinearInOut(float t, float b, float c, float d);
// Sine Easing functions
float EaseSineIn(float t, float b, float c, float d);
float EaseSineOut(float t, float b, float c, float d);
float EaseSineInOut(float t, float b, float c, float d);
// Circular Easing functions
float EaseCircIn(float t, float b, float c, float d);
float EaseCircOut(float t, float b, float c, float d);
float EaseCircInOut(float t, float b, float c, float d);
// Cubic Easing functions
float EaseCubicIn(float t, float b, float c, float d);
float EaseCubicOut(float t, float b, float c, float d);
float EaseCubicInOut(float t, float b, float c, float d);
// Quadratic Easing functions
float EaseQuadIn(float t, float b, float c, float d);
float EaseQuadOut(float t, float b, float c, float d);
float EaseQuadInOut(float t, float b, float c, float d);
// Exponential Easing functions
float EaseExpoIn(float t, float b, float c, float d);
float EaseExpoOut(float t, float b, float c, float d);
float EaseExpoInOut(float t, float b, float c, float d);
// Back Easing functions
float EaseBackIn(float t, float b, float c, float d);
float EaseBackOut(float t, float b, float c, float d);
float EaseBackInOut(float t, float b, float c, float d);
// Bounce Easing functions
float EaseBounceOut(float t, float b, float c, float d);
float EaseBounceIn(float t, float b, float c, float d);
float EaseBounceInOut(float t, float b, float c, float d);
// Elastic Easing functions
float EaseElasticIn(float t, float b, float c, float d);
float EaseElasticOut(float t, float b, float c, float d);
float EaseElasticInOut(float t, float b, float c, float d);

// MARK: Random

typedef struct {
    unsigned int seed;
    int p1, p2;
    unsigned int buffer[17];
} Random;

void InitRandom(Random *r, unsigned int s);
unsigned int RandomBits(Random *r);
float RandomFloat(Random *r);
double RandomDouble(Random *r);
int RandomInt(Random *r, int max);
float RandomFloatRange(Random *r, float min, float max);
double RandomDoubleRange(Random *r, double min, double max);
int RandomIntRange(Random *r, int min, int max);

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

#define ENTITY_ID(E) \
    ((E).parts.id)
#define ENTITY_VERSION(E) \
    ((E).parts.version)
#define ENTITY_IS_NIL(E) \
    ((E).parts.id == EcsNil)
#define ENTITY_CMP(A, B) \
    ((A).id == (B).id)

#define EcsNil 0xFFFFFFFFull
#define EcsNilEntity (Entity){.id = EcsNil}

#define ECS_COMPONENT(T) EcsNewComponent(sizeof(T))
#define ECS_TAG(WORLD) EcsNewComponent(0)
#define ECS_QUERY(CB, UD, ...) EcsQuery(CB, UD, (Entity[]){__VA_ARGS__}, sizeof((Entity[]){__VA_ARGS__}) / sizeof(Entity));
#define ECS_FIELD(QUERY, T, IDX) (T*)EcsQueryField(QUERY, IDX)
#define ECS_SYSTEM(CB, ...) EcsNewSystem(CB, N_ARGS(__VA_ARGS__), __VA_ARGS__)
#define ECS_PREFAB(...) EcsNewPrefab(N_ARGS(__VA_ARGS__), __VA_ARGS__)
#define ECS_CHILDREN(PARENT, CB) (EcsRelations((PARENT), EcsChildof, (CB)))
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

extern EcsWorld world;

typedef struct {
    void **componentData;
    Entity *componentIndex;
    size_t sizeOfComponentData;
    Entity entity;
    void *userdata;
} Query;

typedef void(*SystemCb)(Query *query);

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

typedef void(*TimerCb)(void*);

typedef struct {
    uint64_t start;
    int interval;
    TimerCb cb;
    void *userdata;
    bool enabled;
} Timer;

typedef enum {
    EcsEntityType    = 0,
    EcsComponentType = (1 << 0),
    EcsSystemType    = (1 << 1),
    EcsPrefabType    = (1 << 2),
    EcsRelationType  = (1 << 3),
    EcsTimerType     = (1 << 4)
} EntityFlag;

// MARK: ECS Functions

Entity EcsNewEntity(void);
Entity EcsNewComponent(size_t sizeOfComponent);
Entity EcsNewSystem(SystemCb fn, size_t sizeOfComponents, ...);
Entity EcsNewPrefab(size_t sizeOfComponents, ...);
Entity EcsNewTimer(int interval, bool enable, TimerCb cb, void *userdata);
void DestroyEntity(Entity entity);

bool EcsIsValid(Entity entity);
bool EcsHas(Entity entity, Entity component);
void EcsAttach(Entity entity, Entity component);
void EcsAssociate(Entity entity, Entity object, Entity relation);
void EcsDetach(Entity entity, Entity component);
void EcsDisassociate(Entity entity);
bool EcsHasRelation(Entity entity, Entity object);
bool EcsRelated(Entity entity, Entity relation);
void* EcsGet(Entity entity, Entity component);
void EcsSet(Entity entity, Entity component, const void *data);
void EcsRelations(Entity parent, Entity relation, void *userdata, SystemCb cb);

void EcsEnableSystem(Entity system);
void EcsDisableSystem(Entity system);
void EcsEnableTimer(Entity timer);
void EcsDisableTimer(Entity timer);

void EcsRunSystem(Entity system);
void EcsStep(void);
void EcsQuery(SystemCb cb, void *userdata, Entity *components, size_t sizeOfComponents);
void* EcsQueryField(Query *query, size_t index);

extern Entity EcsSystem;
extern Entity EcsPrefab;
extern Entity EcsRelation;
extern Entity EcsChildOf;
extern Entity EcsTimer;

//! MARK: Image

int RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
int RGB(unsigned char r, unsigned char g, unsigned char b);
int RGBA1(unsigned char c, unsigned char a);
int RGB1(unsigned char c);
unsigned char Rgba(int c);
unsigned char rGba(int c);
unsigned char rgBa(int c);
unsigned char rgbA(int c);
int rGBA(int c, unsigned char r);
int RgBA(int c, unsigned char g);
int RGbA(int c, unsigned char b);
int RGBa(int c, unsigned char a);

typedef struct {
    int *buf, w, h;
} Image;

bool InitImage(Image *img, unsigned int w, unsigned int h);
void DestroyImage(Image *img);

void FillImage(Image *img, int col);
void FloodImage(Image *img, int x, int y, int col);
void ClearImage(Image *img);
void BSet(Image *img, int x, int y, int col);
void PSet(Image *img, int x, int y, int col);
int PGet(Image *img, int x, int y);
bool PasteImage(Image *dst, Image *src, int x, int y);
bool PasteImageClip(Image *dst, Image *src, int x, int y, int rx, int ry, int rw, int rh);
bool CopyImage(Image *a, Image *b);
void PassthruImage(Image *img, int(*fn)(int x, int y, int col));
bool ScaleImage(Image *a, int nw, int nh, Image *img);
bool RotateImage(Image *a, float angle, Image *b);
void DrawLine(Image *img, int x0, int y0, int x1, int y1, int col);
void DrawCircle(Image *img, int xc, int yc, int r, int col, bool fill);
void DrawRect(Image *img, int x, int y, int w, int h, int col, bool fill);
void DrawTri(Image *img, int x0, int y0, int x1, int y1, int x2, int y2, int col, bool fill);

bool LoadImage(Image *out, const char *path);
bool LoadImageMemory(Image *out, const void *data, size_t length);
bool SaveImage(Image *img, const char *path);

#if defined(__cplusplus)
}
#endif
#endif // wee_h
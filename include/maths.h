//
//  maths.h
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#ifndef maths_h
#define maths_h
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#if !defined(__clang__) && (!defined(__GNUC__) || !defined(__GNUG__))
#error This library relies on Clang/GCC extensions! Unsupported compiler!
#endif

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

typedef struct {
    vec2 position, texcoord;
    vec4 color;
} Vertex;

typedef struct {
    float x, y, w, h;
} Rectangle;

#endif /* maths_h */

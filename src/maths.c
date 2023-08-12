//
//  maths.c
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#include "wee.h"

#define X(W, H)                               \
    __MATRIX_T(W, H)                          \
    __MATRIX_D(W, H, Identity)                \
    (void)                                    \
    {                                         \
        __MATRIX_T(W, H)                      \
        m;                                    \
        memset(&m, 0, sizeof(float) * W * H); \
        if (W == H)                           \
            for (int i = 0; i < W; i++)       \
                m[i][i] = 1.f;                \
        return m;                             \
    }                                         \
    __MATRIX_T(W, H)                          \
    __MATRIX_D(W, H, Zero)                    \
    (void)                                    \
    {                                         \
        __MATRIX_T(W, H)                      \
        m;                                    \
        memset(&m, 0, sizeof(float) * W * H); \
        return m;                             \
    }
MATRIX_TYPES
#undef X

#define X(N)                                                        \
    Matrix##N Matrix##N##Identity(void) {                           \
        return (Matrix##N) { .mat = __MATRIX_D(N, N, Identity)() }; \
    }
VECTOR_TYPES
#undef X

#define X(L)                                                \
    __VEC_T(L)                                              \
    __VEC_D(L, Zero)                                        \
    (void)                                                  \
    {                                                       \
        __VEC_T(L)                                          \
        v;                                                  \
        memset(&v, 0, sizeof(float) * L);                   \
        return v;                                           \
    }                                                       \
    __VEC_T(L)                                              \
    __VEC_D(L, New)                                         \
    (float x, ...)                                          \
    {                                                       \
        va_list args;                                       \
        va_start(args, L - 1);                              \
        __VEC_T(L)                                          \
        v;                                                  \
        v[0] = x;                                           \
        for (int i = 0; i < L - 1; i++)                     \
            v[i + 1] = (float)va_arg(args, double);         \
        va_end(args);                                       \
        return v;                                           \
    }                                                       \
    void __VEC_D(L, Print)(__VEC_T(L) v)                    \
    {                                                       \
        puts("{ ");                                         \
        for (int i = 0; i < L; i++)                         \
            printf("%f ", v[i]);                            \
        puts("}");                                          \
    }                                                       \
    float __VEC_D(L, Sum)(__VEC_T(L) v)                     \
    {                                                       \
        float r = 0.f;                                      \
        for (int i = 0; i < L; i++)                         \
            r += v[i];                                      \
        return r;                                           \
    }                                                       \
    float __VEC_D(L, LengthSqr)(__VEC_T(L) v)               \
    {                                                       \
        return __VEC_D(L, Sum)(v * v);                      \
    }                                                       \
    float __VEC_D(L, Length)(__VEC_T(L) v)                  \
    {                                                       \
        return sqrtf(__VEC_D(L, LengthSqr)(v));             \
    }                                                       \
    float __VEC_D(L, Dot)(__VEC_T(L) v1, __VEC_T(L) v2)     \
    {                                                       \
        return __VEC_D(L, Sum)(v1 * v2);                    \
    }                                                       \
    __VEC_T(L)                                              \
    __VEC_D(L, Normalize)                                   \
    (__VEC_T(L) v)                                          \
    {                                                       \
        __VEC_T(L)                                          \
        r = {0};                                            \
        float l = __VEC_D(L, Length)(v);                    \
        for (int i = 0; i < L; i++)                         \
            r[i] = v[i] * (1.f / l);                        \
        return r;                                           \
    }                                                       \
    float __VEC_D(L, DistSqr)(__VEC_T(L) v1, __VEC_T(L) v2) \
    {                                                       \
        __VEC_T(L)                                          \
        v = v2 - v1;                                        \
        return __VEC_D(L, Sum)(v * v);                      \
    }                                                       \
    float __VEC_D(L, Dist)(__VEC_T(L) v1, __VEC_T(L) v2)    \
    {                                                       \
        return sqrtf(__VEC_D(L, DistSqr)(v1, v2));          \
    }                                                       \
    __VEC_T(L)                                              \
    __VEC_D(L, Clamp)                                       \
    (__VEC_T(L) v, __VEC_T(L) min, __VEC_T(L) max)          \
    {                                                       \
        __VEC_T(L)                                          \
        r;                                                  \
        for (int i = 0; i < L; i++)                         \
            r[i] = Clamp(v[i], min[i], max[i]);             \
        return r;                                           \
    }                                                       \
    __VEC_T(L)                                              \
    __VEC_D(L, Min)                                         \
    (__VEC_T(L) v, __VEC_T(L) min)                          \
    {                                                       \
        __VEC_T(L)                                          \
        r;                                                  \
        for (int i = 0; i < L; i++)                         \
            r[i] = Min(v[i], min[i]);                       \
        return r;                                           \
    }                                                       \
    __VEC_T(L)                                              \
    __VEC_D(L, Max)                                         \
    (__VEC_T(L) v, __VEC_T(L) max)                          \
    {                                                       \
        __VEC_T(L)                                          \
        r;                                                  \
        for (int i = 0; i < L; i++)                         \
            r[i] = Max(v[i], max[i]);                       \
        return r;                                           \
    }                                                       \
    __VEC_T(L)                                              \
    __VEC_D(L, Lerp)                                        \
    (__VEC_T(L) v1, __VEC_T(L) v2, float n)                 \
    {                                                       \
        return v1 + n * (v2 - v1);                          \
    }
VECTOR_TYPES
#undef X

float Vec2Angle(Vec2 v1, Vec2 v2) {
    return atan2f(v2.y, v2.x) - atan2f(v1.y, v1.x);
}

Vec2 Vec2Rotate(Vec2 v, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    return (Vec2) { v.x*c - v.y*s,  v.x*s + v.y*c };
}

Vec2 Vec2MoveTowards(Vec2 v, Vec2 target, float maxDistance) {
    Vec2 delta = target - v;
    float dist = Vec2LengthSqr(delta);
    if (!dist || (maxDistance >= 0 && dist <= maxDistance * maxDistance))
        return target;
    else
        return v + (delta / (sqrtf(dist) * maxDistance));
}

Vec2 Vec2Reflect(Vec2 v, Vec2 normal) {
    return v - (2.f * normal) * Vec2Dot(v, normal);
}

Vec3 Vec3Reflect(Vec3 v, Vec3 normal) {
    return v - (2.f * normal) * Vec3Dot(v, normal);
}

Vec3 Vec3Cross(Vec3 v1, Vec3 v2) {
    return (Vec3) { v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x };
}

Vec3 Vec3Perpendicular(Vec3 v) {
    float min = (float) fabs(v.x);
    Vec3 a = {1.0f, 0.0f, 0.0f};
    if (fabsf(v.y) < min) {
        min = (float) fabs(v.y);
        a = (Vec3){0.0f, 1.0f, 0.0f};
    }
    if (fabsf(v.z) < min)
        a = (Vec3){0.0f, 0.0f, 1.0f};
    return (Vec3) { v.y*a.z - v.z*a.y,
                    v.z*a.x - v.x*a.z,
                    v.x*a.y - v.y*a.x };
}

float Vec3Angle(Vec3 v1, Vec3 v2) {
    return atan2f(Vec3Length(Vec3Cross(v1, v2)), Vec3Dot(v1, v2));
}

Vec3 Vec3RotateByQuaternion(Vec3 v, Quat q) {
    return (Vec3) {
        v.x*(q.x*q.x + q.w*q.w - q.y*q.y - q.z*q.z) + v.y*(2*q.x*q.y - 2*q.w*q.z) + v.z*(2*q.x*q.z + 2*q.w*q.y),
        v.x*(2*q.w*q.z + 2*q.x*q.y) + v.y*(q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z) + v.z*(-2*q.w*q.x + 2*q.y*q.z),
        v.x*(-2*q.w*q.y + 2*q.x*q.z) + v.y*(2*q.w*q.x + 2*q.y*q.z)+ v.z*(q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z)
    };
}

Vec3 Vec3RotateByAxisAngle(Vec3 v, Vec3 axis, float angle) {
    // Using Euler-Rodrigues Formula
    // Ref.: https://en.wikipedia.org/w/index.php?title=Euler%E2%80%93Rodrigues_formula

    axis *= (1.0f / (Clamp(Vec3Length(axis), 0.f, 1.f)));

    angle /= 2.0f;
    Vec3 w = axis * sinf(angle);
    Vec3 wv = Vec3Cross(w, v);
    return v + (wv * (cosf(angle) * 2.f)) + (Vec3Cross(w, wv) * 2.f);
}

Vec3 Vec3Refract(Vec3 v, Vec3 n, float r) {
    float dot = Vec3Dot(v, n);
    float d = 1.0f - r*r*(1.0f - dot*dot);
    return d < 0 ? Vec3Zero() : r * v - (r * dot + sqrtf(d)) * n;
}

int FloatCmp(float a, float b) {
    return (fabsf(a - b)) <= (EPSILON * fmaxf(1.0f, fmaxf(fabsf(a), fabsf(b))));
}

int Min(int a, int b) {
    return a < b ? a : b;
}

int Max(int a, int b) {
    return a > b ? a : b;
}

int Clamp(int n, int min, int max) {
    return Min(Max(n, min), max);
}

float Degrees(float radians) {
    return radians * (180.0f / PI);
}

float Radians(float degrees) {
    return degrees * (PI / 180.0f);
}

float EaseLinearNone(float t, float b, float c, float d) {
    return (c * t / d + b);
}

float EaseLinearIn(float t, float b, float c, float d) {
    return (c * t / d + b);
}

float EaseLinearOut(float t, float b, float c, float d) {
    return (c * t / d + b);
}

float EaseLinearInOut(float t, float b, float c, float d) {
    return (c * t / d + b);
}

float EaseSineIn(float t, float b, float c, float d) {
    return (-c * cosf(t / d * (PI / 2.0f)) + c + b);
}

float EaseSineOut(float t, float b, float c, float d) {
    return (c * sinf(t / d * (PI / 2.0f)) + b);
}

float EaseSineInOut(float t, float b, float c, float d) {
    return (-c / 2.0f * (cosf(PI * t / d) - 1.0f) + b);
}

float EaseCircIn(float t, float b, float c, float d) {
    t /= d;
    return (-c * (sqrtf(1.0f - t * t) - 1.0f) + b);
}

float EaseCircOut(float t, float b, float c, float d) {
    t = t / d - 1.0f;
    return (c * sqrtf(1.0f - t * t) + b);
}

float EaseCircInOut(float t, float b, float c, float d) {
    if ((t /= d / 2.0f) < 1.0f)
        return (-c / 2.0f * (sqrtf(1.0f - t * t) - 1.0f) + b);
    t -= 2.0f;
    return (c / 2.0f * (sqrtf(1.0f - t * t) + 1.0f) + b);
}

float EaseCubicIn(float t, float b, float c, float d) {
    t /= d;
    return (c * t * t * t + b);
}

float EaseCubicOut(float t, float b, float c, float d) {
    t = t / d - 1.0f;
    return (c * (t * t * t + 1.0f) + b);
}

float EaseCubicInOut(float t, float b, float c, float d) {
    if ((t /= d / 2.0f) < 1.0f)
        return (c / 2.0f * t * t * t + b);
    t -= 2.0f;
    return (c / 2.0f * (t * t * t + 2.0f) + b);
}

float EaseQuadIn(float t, float b, float c, float d) {
    t /= d;
    return (c * t * t + b);
}

float EaseQuadOut(float t, float b, float c, float d) {
    t /= d;
    return (-c * t * (t - 2.0f) + b);
}

float EaseQuadInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1)
        return (((c / 2) * (t * t)) + b);
    return (-c / 2.0f * (((t - 1.0f) * (t - 3.0f)) - 1.0f) + b);
}

float EaseExpoIn(float t, float b, float c, float d) {
    return (t == 0.0f) ? b : (c * powf(2.0f, 10.0f * (t / d - 1.0f)) + b);
}

float EaseExpoOut(float t, float b, float c, float d) {
    return (t == d) ? (b + c) : (c * (-powf(2.0f, -10.0f * t / d) + 1.0f) + b);
}

float EaseExpoInOut(float t, float b, float c, float d) {
    if (t == 0.0f)
        return b;
    if (t == d)
        return (b + c);
    if ((t /= d / 2.0f) < 1.0f)
        return (c / 2.0f * powf(2.0f, 10.0f * (t - 1.0f)) + b);

    return (c / 2.0f * (-powf(2.0f, -10.0f * (t - 1.0f)) + 2.0f) + b);
}

float EaseBackIn(float t, float b, float c, float d) {
    float s = 1.70158f;
    float postFix = t /= d;
    return (c * (postFix)*t * ((s + 1.0f) * t - s) + b);
}

float EaseBackOut(float t, float b, float c, float d) {
    float s = 1.70158f;
    t = t / d - 1.0f;
    return (c * (t * t * ((s + 1.0f) * t + s) + 1.0f) + b);
}

float EaseBackInOut(float t, float b, float c, float d) {
    float s = 1.70158f;
    if ((t /= d / 2.0f) < 1.0f) {
        s *= 1.525f;
        return (c / 2.0f * (t * t * ((s + 1.0f) * t - s)) + b);
    }

    float postFix = t -= 2.0f;
    s *= 1.525f;
    return (c / 2.0f * ((postFix)*t * ((s + 1.0f) * t + s) + 2.0f) + b);
}

float EaseBounceOut(float t, float b, float c, float d) {
    if ((t /= d) < (1.0f / 2.75f)) {
        return (c * (7.5625f * t * t) + b);
    } else if (t < (2.0f / 2.75f)) {
        float postFix = t -= (1.5f / 2.75f);
        return (c * (7.5625f * (postFix)*t + 0.75f) + b);
    } else if (t < (2.5 / 2.75)) {
        float postFix = t -= (2.25f / 2.75f);
        return (c * (7.5625f * (postFix)*t + 0.9375f) + b);
    } else {
        float postFix = t -= (2.625f / 2.75f);
        return (c * (7.5625f * (postFix)*t + 0.984375f) + b);
    }
}

float EaseBounceIn(float t, float b, float c, float d) {
    return (c - EaseBounceOut(d - t, 0.0f, c, d) + b);
    
}

float EaseBounceInOut(float t, float b, float c, float d) {
    if (t < d / 2.0f)
        return (EaseBounceIn(t * 2.0f, 0.0f, c, d) * 0.5f + b);
    else
        return (EaseBounceOut(t * 2.0f - d, 0.0f, c, d) * 0.5f + c * 0.5f + b);
}

float EaseElasticIn(float t, float b, float c, float d) {
    if (t == 0.0f)
        return b;
    if ((t /= d) == 1.0f)
        return (b + c);

    float p = d * 0.3f;
    float a = c;
    float s = p / 4.0f;
    float postFix = a * powf(2.0f, 10.0f * (t -= 1.0f));

    return (-(postFix * sinf((t * d - s) * (2.0f * PI) / p)) + b);
}

float EaseElasticOut(float t, float b, float c, float d) {
    if (t == 0.0f)
        return b;
    if ((t /= d) == 1.0f)
        return (b + c);

    float p = d * 0.3f;
    float a = c;
    float s = p / 4.0f;

    return (a * powf(2.0f, -10.0f * t) * sinf((t * d - s) * (2.0f * PI) / p) + c + b);
}

float EaseElasticInOut(float t, float b, float c, float d) {
    if (t == 0.0f)
        return b;
    if ((t /= d / 2.0f) == 2.0f)
        return (b + c);

    float p = d * (0.3f * 1.5f);
    float a = c;
    float s = p / 4.0f;

    if (t < 1.0f) {
        float postFix = a * powf(2.0f, 10.0f * (t -= 1.0f));
        return -0.5f * (postFix * sinf((t * d - s) * (2.0f * PI) / p)) + b;
    }

    float postFix = a * powf(2.0f, -10.0f * (t -= 1.0f));

    return (postFix * sinf((t * d - s) * (2.0f * PI) / p) * 0.5f + c + b);
}

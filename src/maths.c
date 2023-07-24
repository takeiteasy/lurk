//
//  maths.c
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#include "maths.h"

#define X(N)                                               \
    vec##N Vec##N##Zero(void)                              \
    {                                                      \
        vec##N v = {0};                                    \
        for (int i = 0; i < N; i++)                        \
            v[i] = 0;                                      \
        return v;                                          \
    }                                                      \
    static float Vec##N##Sum(vec##N v)                     \
    {                                                      \
        float r = 0.f;                                     \
        for (int i = 0; i < N; i++)                        \
            r += v[i];                                     \
        return r;                                          \
    }                                                      \
    float Vec##N##LengthSqr(vec##N v)                      \
    {                                                      \
        return Vec##N##Sum(v * v);                         \
    }                                                      \
    float Vec##N##Length(vec##N v)                         \
    {                                                      \
        return sqrtf(Vec##N##LengthSqr(v));                \
    }                                                      \
    float Vec##N##Dot(vec##N v1, vec##N v2)                \
    {                                                      \
        return Vec##N##Sum(v1 * v2);                       \
    }                                                      \
    vec##N Vec##N##Normalize(vec##N v)                     \
    {                                                      \
        vec##N r = {0};                                    \
        float l = Vec##N##Length(v);                       \
        for (int i = 0; i < N; i++)                        \
            r[i] = v[i] * (1.f / l);                       \
        return r;                                          \
    }
SIZES
#undef X

vec3 Vec3Cross(vec3 v1, vec3 v2) {
    return (vec3) { v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x };
}

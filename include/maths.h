//
//  maths.h
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#ifndef maths_h
#define maths_h
#define _USE_MATH_DEFINES
#include <math.h>

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define CLAMP(n, min, max) (MIN(MAX(n, min), max))

#define SIZES \
    X(2)      \
    X(3)      \
    X(4)

#define X(N)                                                          \
    typedef int pos##N __attribute__((ext_vector_type(N)));           \
    typedef float vec##N __attribute__((ext_vector_type(N)));         \
    vec##N Vec##N##Zero(void);                                        \
    float Vec##N##LengthSqr(vec##N v);                                \
    float Vec##N##Length(vec##N v);                                   \
    float Vec##N##Dot(vec##N v1, vec##N v2);                          \
    vec##N Vec##N##Normalize(vec##N v);
SIZES
#undef X
#define Vec2(X, Y) (vec2){(X), (Y)}
#define Vec3(X, Y, Z) (vec3){(X), (Y), (Z)}
#define Vec4(X, Y, Z, W) (vec4){(X), (Y), (Z), (W)}
typedef vec4 quat;
#define Quat(...) Vec4(__VA_ARGS__)

vec3 Vec3Cross(vec3 v1, vec3 v2);

typedef struct {
    vec2 position, texcoord;
    vec4 color;
} Vertex;

typedef struct {
    float x, y, w, h;
} Rectangle;

#endif /* maths_h */

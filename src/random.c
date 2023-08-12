//
//  random.c
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#include "wee.h"

void InitRandom(Random *r, unsigned int s) {
    r->seed = s;
    for (int i = 0; i < 17; i++) {
        s = s * 0xAC564B05 + 1;
        r->buffer[i] = s;
    }
    r->p1 = 0;
    r->p2 = 10;
}

#define ROTL(N, R) (((N) << (R)) | ((N) >> (32 - (R))))

unsigned int RandomBits(Random *r) {
    unsigned int result = r->buffer[r->p1] = ROTL(r->buffer[r->p2], 13) + ROTL(r->buffer[r->p1], 9);

    if (--r->p1 < 0)
        r->p1 = 16;
    if (--r->p2 < 0)
        r->p2 = 16;

    return result;
}

float RandomFloat(Random *r) {
    union {
        float value;
        unsigned int word;
    } convert = {
        .word = (RandomBits(r) >> 9) | 0x3f800000};
    return convert.value - 1.0f;
}

double RandomDouble(Random *r) {
    unsigned int bits = RandomBits(r);
    union {
        double value;
        unsigned int words[2];
    } convert = {
        .words = {
            bits << 20,
            (bits >> 12) | 0x3FF00000}};
    return convert.value - 1.0;
}

int RandomInt(Random *r, int max) {
    return RandomBits(r) % max;
}

float RandomFloatRange(Random *r, float min, float max) {
    return RandomFloat(r) * (max - min) + min;
}

double RandomDoubleRange(Random *r, double min, double max) {
    return RandomDouble(r) * (max - min) + min;
}

int RandomIntRange(Random *r, int min, int max) {
    return RandomBits(r) % (max + 1 - min) + min;
}

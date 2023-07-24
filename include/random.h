//
//  random.h
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#ifndef random_h
#define random_h
#if defined(__cplusplus)
extern "C" {
#endif

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

#if defined(__cplusplus)
}
#endif
#endif /* random_h */

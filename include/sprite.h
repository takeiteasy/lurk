//
//  sprite.h
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#ifndef sprite_h
#define sprite_h
#include "sokol_gfx.h"
#include "sprite.glsl.h"
#include "maths.h"
#include "image.h"

typedef struct {
    sg_image sprite;
    int w, h;
} Sprite;

typedef struct {
    Vertex *vertices;
    int maxVertices, vertexCount;
    sg_bindings bind;
    int w, h;
} Spritebatch;

void InitSprite(Sprite *sprite, Image *img);
void InitMutableSprite(Sprite *sprite, int w, int h);
void DestroySprite(Sprite *sprite);

void InitSpritebatch(Spritebatch *sb, int max, Sprite *sprite);
void SpritebatchBegin(Spritebatch *sb);
void SpritebatchRender(Spritebatch *sb, int rw, int rh, Rectangle bounds, Rectangle clip, vec4 color);
void SpritebatchEnd(Spritebatch *sb);
void DestroySpritebatch(Spritebatch *sb);

#endif /* sprite_h */

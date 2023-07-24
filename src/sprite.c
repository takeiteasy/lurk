//
//  sprite.c
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#include "sprite.h"

void InitSprite(Sprite *sprite, Image *img) {
    sprite->w = img->w;
    sprite->h = img->h;
    sg_image_desc desc = (sg_image_desc) {
        .width = img->w,
        .height = img->h,
        .data.subimage[0][0] = (sg_range){
            .ptr=img->buf,
            .size=img->w * img->h * sizeof(int)
        }
    };
    sprite->sprite = sg_make_image(&desc);
}

void InitMutableSprite(Sprite *sprite, int w, int h) {
    sprite->w = w;
    sprite->h = h;
    sg_image_desc desc = (sg_image_desc) {
        .width = w,
        .height = h,
        .usage = SG_USAGE_STREAM
    };
    sprite->sprite = sg_make_image(&desc);
}

void DestroySprite(Sprite *sprite) {
    if (sprite && sg_query_image_state(sprite->sprite) == SG_RESOURCESTATE_VALID)
        sg_destroy_image(sprite->sprite);
}

void InitSpritebatch(Spritebatch *sb, int max, Sprite *sprite) {
    sb->maxVertices = max;
    sb->vertexCount = 0;
    sb->w = sprite->w;
    sb->h = sprite->h;
    size_t sz = 6 * max * sizeof(Vertex);
    sb->vertices = malloc(sz);
    sg_buffer_desc desc = (sg_buffer_desc) {
        .usage = SG_USAGE_STREAM,
        .size = sz
    };
    sb->bind = (sg_bindings) {
        .vertex_buffers[0] = sg_make_buffer(&desc),
        .fs_images[SLOT_sprite] = sprite->sprite
    };
}

void SpritebatchBegin(Spritebatch *sb) {
    if (!sb)
        return;
    memset(sb->vertices, 0, 6 * sb->maxVertices * sizeof(Vertex));
    sb->vertexCount = 0;
}

void SpritebatchRender(Spritebatch *sb, int w, int h, Rectangle rect, Rectangle clip, vec4 color) {
    vec2 quad[4] = {
        {rect.x,          rect.y + rect.h}, // bottom left
        {rect.x + rect.w, rect.y + rect.h}, // bottom right
        {rect.x + rect.w, rect.y         }, // top right
        {rect.x,          rect.y         }, // top left
    };
    float vw =  2.f / (float)w;
    float vh = -2.f / (float)h;
    for (int j = 0; j < 4; j++)
        quad[j] = (vec2) {
            vw * quad[j].x + -1.f,
            vh * quad[j].y +  1.f
        };
    
    Vertex *v = &sb->vertices[sb->vertexCount++ * 6];
    v[0].position = quad[0];
    v[1].position = quad[1];
    v[2].position = quad[2];
    v[3].position = quad[3];
    v[4].position = quad[0];
    v[5].position = quad[2];
    
    for (int i = 0; i < 6; i++)
        v[i].color = color;
    
    float iw = 1.f/sb->w, ih = 1.f/(float)sb->h;
    float tl = clip.x*iw;
    float tt = clip.y*ih;
    float tr = (clip.x + clip.w)*iw;
    float tb = (clip.y + clip.h)*ih;
    vec2 vtexquad[4] = {
        {tl, tb}, // bottom left
        {tr, tb}, // bottom right
        {tr, tt}, // top right
        {tl, tt}, // top left
    };
    
    v[0].texcoord = vtexquad[0];
    v[1].texcoord = vtexquad[1];
    v[2].texcoord = vtexquad[2];
    v[3].texcoord = vtexquad[3];
    v[4].texcoord = vtexquad[0];
    v[5].texcoord = vtexquad[2];
}

void SpritebatchEnd(Spritebatch *sb) {
    if (!sb || !sb->vertexCount)
        return;
    sg_range range = (sg_range) {
        .ptr = sb->vertices,
        .size = 6 * sb->vertexCount * sizeof(Vertex)
    };
    sg_update_buffer(sb->bind.vertex_buffers[0], &range);
    sg_apply_bindings(&sb->bind);
    sg_draw(0, 6 * sb->vertexCount, 1);
}

void DestroySpritebatch(Spritebatch *sb) {
    if (!sb)
        return;
    if (sb->vertices)
        free(sb->vertices);
    if (sg_query_buffer_state(sb->bind.vertex_buffers[0]) == SG_RESOURCESTATE_VALID)
        sg_destroy_buffer(sb->bind.vertex_buffers[0]);
}

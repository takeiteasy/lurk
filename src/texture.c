//
//  texture.c
//  wee
//
//  Created by George Watson on 24/08/2023.
//

#include "wee.h"
#include "texture.glsl.h"

static Texture* NewTexture(sg_image_desc *desc) {
    Texture *result = malloc(sizeof(Texture));
    result->internal = sg_make_image(desc);
    result->w = desc->width;
    result->h = desc->height;
    return result;
}

Texture* LoadTextureFromImage(ezImage *img) {
    Texture *result = CreateEmptyTexture(img->w, img->h);
    UpdateTexture(result, img);
    return result;
}

Texture* LoadTextureFromFile(const char *path) {
    ezImage *img = ezImageLoadFromPath(path);
    Texture *result = LoadTextureFromImage(img);
    ezImageFree(img);
    return result;
}

Texture* CreateEmptyTexture(unsigned int w, unsigned int h) {
    sg_image_desc desc = {
        .width = w,
        .height = h,
        .usage = SG_USAGE_STREAM
    };
    return NewTexture(&desc);
}

void UpdateTexture(Texture *texture, ezImage *img) {
    if (texture->w != img->w || texture->h != img->h) {
        DestroyTexture(texture);
        texture = CreateEmptyTexture(img->w, img->h);
    }
    sg_image_data data = {
        .subimage[0][0] = (sg_range) {
            .ptr = img->buf,
            .size = img->w * img->h * sizeof(int)
        }
    };
    sg_update_image(texture->internal, &data);
}

typedef Vertex Quad[6];

static void GenerateQuad(Vec2f position, Vec2f textureSize, Vec2f size, Vec2f scale, Vec2f viewportSize, float rotation, Rect clip, Quad *out) {
    Vec2f quad[4] = {
        {position.x, position.y + size.y}, // bottom left
        {position.x + size.x, position.y + size.y}, // bottom right
        {position.x + size.x, position.y }, // top right
        {position.x, position.y }, // top left
    };
    float vw =  2.f / (float)viewportSize.x;
    float vh = -2.f / (float)viewportSize.y;
    for (int j = 0; j < 4; j++)
        quad[j] = (Vec2f) {
            (vw * quad[j].x + -1.f) * scale.x,
            (vh * quad[j].y +  1.f) * scale.y
        };
    
    float iw = 1.f/textureSize.x, ih = 1.f/(float)textureSize.y;
    float tl = clip.x*iw;
    float tt = clip.y*ih;
    float tr = (clip.x + clip.w)*iw;
    float tb = (clip.y + clip.h)*ih;
    Vec2f vtexquad[4] = {
        {tl, tb}, // bottom left
        {tr, tb}, // bottom right
        {tr, tt}, // top right
        {tl, tt}, // top left
    };
    static int indices[6] = {
        0, 1, 2,
        3, 0, 2
    };
    
    for (int i = 0; i < 6; i++)
        (*out)[i] = (Vertex) {
            .position = quad[indices[i]],
            .texcoord = vtexquad[indices[i]],
            .color = {1.f, 1.f, 1.f, 1.f}
        };
}

void DrawTexture(Texture *texture, Vec2f position, Vec2f size, Vec2f scale, Vec2f viewportSize, float rotation, Rect clip) {
    Quad quad;
    GenerateQuad(position, (Vec2f){texture->w, texture->h}, size, scale, viewportSize, rotation, clip, &quad);
    sg_buffer_desc desc = {
        .data = SG_RANGE(quad)
    };
    sg_bindings bind = {
        .vertex_buffers[0] = sg_make_buffer(&desc),
        .fs_images[SLOT_tex] = texture->internal
    };
    sg_apply_bindings(&bind);
    sg_draw(0, 6, 1);
    sg_destroy_buffer(bind.vertex_buffers[0]);
}

void DestroyTexture(Texture *texture) {
    if (texture) {
        if (sg_query_image_state(texture->internal) == SG_RESOURCESTATE_VALID)
            sg_destroy_image(texture->internal);
        free(texture);
    }
}

TextureBatch* CreateTextureBatch(Texture *texture, int max) {
    TextureBatch *result = malloc(sizeof(TextureBatch));
    result->maxVertices = max * 6;
    result->vertexCount = 0;
    result->size = (Vec2f){texture->w, texture->h};
    result->vertices = malloc(result->maxVertices * sizeof(Vertex));
    result->texture = texture;
    sg_buffer_desc desc = {
        .usage = SG_USAGE_STREAM,
        .size = result->maxVertices * sizeof(Vertex)
    };
    result->bind = (sg_bindings) {
        .vertex_buffers[0] = sg_make_buffer(&desc),
        .fs_images[SLOT_tex] = texture->internal
    };
    return result;
}

void ResizeTextureBatch(TextureBatch **batch, int newMaxVertices) {
    TextureBatch *_batch = *batch;
    assert(batch && _batch->texture && sg_query_image_state(_batch->texture->internal) == SG_RESOURCESTATE_VALID);
    TextureBatch *new = CreateTextureBatch(_batch->texture, newMaxVertices);
    DestroyTextureBatch(_batch);
    *batch = new;
}

void TextureBatchDraw(TextureBatch *batch, Vec2f position, Vec2f size, Vec2f scale, Vec2f viewportSize, float rotation, Rect clip) {
    GenerateQuad(position, batch->size, size, scale, viewportSize, rotation, clip, (Quad*)(batch->vertices + batch->vertexCount));
    batch->vertexCount += 6;
}

void FlushTextureBatch(TextureBatch *batch) {
    sg_range range = {
        .ptr = batch->vertices,
        .size = batch->vertexCount * sizeof(Vertex)
    };
    sg_update_buffer(batch->bind.vertex_buffers[0], &range);
    sg_apply_bindings(&batch->bind);
    sg_draw(0, batch->vertexCount, 1);
    memset(batch->vertices, 0, batch->maxVertices * sizeof(Vertex));
    batch->vertexCount = 0;
}

void DestroyTextureBatch(TextureBatch *batch) {
    if (batch) {
        if (batch->vertices)
            free(batch->vertices);
        if (sg_query_buffer_state(batch->bind.vertex_buffers[0]) == SG_RESOURCESTATE_VALID)
            sg_destroy_buffer(batch->bind.vertex_buffers[0]);
        free(batch);
    }
}

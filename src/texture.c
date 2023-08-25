//
//  texture.c
//  wee
//
//  Created by George Watson on 24/08/2023.
//

#include "wee.h"
#include "texture.glsl.h"

Texture* LoadTextureFromImage(Image *img) {
    Texture *result = CreateEmptyTexture(img->w, img->h);
    UpdateTexture(result, img);
    return result;
}

Texture* LoadTextureFromFile(const char *path) {
    Image *img = LoadImage(path);
    Texture *result = LoadTextureFromImage(img);
    DestroyImage(img);
    return result;
}

Texture* CreateEmptyTexture(unsigned int w, unsigned int h) {
    sg_image_desc desc = {
        .width = w,
        .height = h,
        .usage = SG_USAGE_STREAM
    };
    Texture *result = malloc(sizeof(Texture));
    result->sgi = sg_make_image(&desc);
    result->w = w;
    result->h = h;
    return result;
}

void UpdateTexture(Texture *texture, Image *img) {
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
    sg_update_image(texture->sgi, &data);
}

void DestroyTexture(Texture *texture) {
    if (texture) {
        if (sg_query_image_state(texture->sgi) == SG_RESOURCESTATE_VALID)
            sg_destroy_image(texture->sgi);
        free(texture);
    }
}

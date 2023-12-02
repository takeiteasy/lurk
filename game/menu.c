#include "gamekit.h"

struct GameKit {
    uint64_t test_a;
    uint64_t test_b;
};

static GameKit* init(gkState* state) {
    GameKit *result = malloc(sizeof(struct GameKit));
    result->test_a = gkFindTexture(state, "test2.png");
    return result;
}

static void deinit(gkState* state, GameKit *scene) {
    free(scene);
}

static void reload(gkState* state, GameKit *scene) {
    
}

static void unload(gkState* state, GameKit *scene) {
    
}

static void event(gkState* state, GameKit *scene, const sapp_event *e) {
    
}

static void frame(gkState* state, GameKit *scene, float delta) {
    int w = state->windowWidth;
    int h = state->windowHeight;
    gkViewport(state, 0, 0, w, h);
//    float ratio = (float)w / (float)h;
//    gkProject(state, ratio, -ratio, 1.f, -1.f);
    
    gkSetImage(state, scene->test_a, 0);
    gkDrawTexturedRect(state, 0, (sgp_rect){0, 0, 256, 256}, (sgp_rect){0, 0, 256, 256});
}

EXPORT const gkScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};

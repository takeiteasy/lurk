#include "gamekit.h"

struct GameKit {
    uint64_t test_a;
    uint64_t test_b;
};

static GameKit* init(gkState* state) {
    GameKit *result = malloc(sizeof(struct GameKit));
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
    gkViewport(state, 0, 0, 640, 480);
    float ratio = 640.f / 480.f;
    gkProject(state, -ratio, ratio, 1.f, -1.f);
    gkSetColor(state, .1f, .1f, .1f, 1.f);
    gkClear(state);
    
    gkSetColor(state, 1.f, 1.f, 0.f, 1.f);
    gkDrawFilledRect(state, -.5f, -.5f, 1.f, 1.f);
}

EXPORT const gkScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};

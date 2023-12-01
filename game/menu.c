#include "gamekit.h"

struct GameKit {
    uint64_t test_a;
    uint64_t test_b;
};

static GameKit* init(gkState* state) {
    printf("Initializing menu scene...\n");
    GameKit *result = malloc(sizeof(struct GameKit));
    result->test_a = gkFindTexture(state, "test.png");
    result->test_b = gkFindTexture(state, "test2.png");
    return result;
}

static void deinit(gkState* state, GameKit *scene) {
    printf("Deinitializing Menu scene\n");
}

static void reload(gkState* state, GameKit *scene) {
    printf("Menu scene reloaded\n");
}

static void unload(gkState* state, GameKit *scene) {
    printf("Menu scene unloaded\n");
}

static void event(gkState* state, GameKit *scene, const sapp_event *e) {
    
}

static void frame(gkState* state, GameKit *scene, float delta) {
    
}

EXPORT const gkScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};

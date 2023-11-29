#include "gamekit.h"

struct GameKit {
    int test;
};

static GameKit* init(gkState *state) {
    printf("Initializing game scene...\n");
    GameKit *result = malloc(sizeof(struct GameKit));
    return result;
}

static void deinit(gkState *state, GameKit *scene) {
    printf("Deinitializing game scene\n");
}

static void reload(gkState *state, GameKit *scene) {
    printf("Game scene reloaded\n");
}

static void unload(gkState *state, GameKit *scene) {
    printf("Game scene unloaded\n");
}

static void event(gkState *state, GameKit *scene, const sapp_event *e) {
    
}

static void frame(gkState *state, GameKit *scene, float delta) {
    printf("hello from game!\n");
}

EXPORT const gkScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};

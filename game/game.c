#include "wee.h"

struct weeeeeeeeeeeeeeeeeee {
    int test;
};

static weeeeeeeeeeeeeeee* init(weeState *state) {
    printf("Initializing game scene...\n");
    weeee *result = malloc(sizeof(struct weeeeeeeeeee));
    result->test = 1;
    return result;
}

static void deinit(weeState *state, weeeeeeeeeeeeeee *scene) {
    printf("Deinitializing game scene\n");
}

static void reload(weeState *state, weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee *scene) {
    printf("Game scene reloaded\n");
}

static void unload(weeState *state, weeeeeeee *scene) {
    printf("Game scene unloaded\n");
}

static void event(weeState *state, weeeeeeeeeeeeeeeeeeeeee *scene, const sapp_event *e) {
    
}

static void frame(weeState *state, weeeeeeeeeeeeeee *scene, float delta) {
    printf("hello from game!\n");
    weePopScene(state);
    return true;
}

EXPORT const weeScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};

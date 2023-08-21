#include "wee.h"

struct weeeeeeeeeeeeeeeeeee {
    int test;
};

static weeeeeeeeeeeeeeee* init(weeState* state) {
    weeInit(state);
    printf("Initializing Menu scene...\n");
    weeee *result = malloc(sizeof(struct weeeeeeeeeee));
    result->test = 1;
    return result;
}

static void deinit(weeState* state, weeeeeeeeeeeeeee *scene) {
    printf("Deinitializing Menu scene\n");
}

static void reload(weeState* state, weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee *scene) {
    printf("Menu scene reloaded\n");
}

static void unload(weeState* state, weeeeeeee *scene) {
    printf("Menu scene unloaded\n");
}

static void event(weeState* state, weeeeeeeeeeeeeeeeeeeeee *scene, const sapp_event *e) {
    
}

static bool frame(weeState* state, weeeeeeeeeeeeeee *scene, float delta) {
    printf("hello from menu!\n");
    weePushScene(state, "game");
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
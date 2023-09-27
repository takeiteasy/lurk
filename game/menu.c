#include "wee.h"

struct weeeeeeeeeeeeeeeeeee {
    int test;
};

static weeeeeeeeeeeeeeee* init(weeState* state) {
    weeInit(state);
    printf("Initializing menu scene...\n");
    weeee *result = malloc(sizeof(struct weeeeeeeeeee));
    result->test = weeFindTexture(state, "test.png");
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

static void frame(weeState* state, weeeeeeeeeeeeeee *scene, float delta) {
    printf("hello from menu?\n");
    weeDrawTexture(state, scene->test);
}

EXPORT const weeScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};

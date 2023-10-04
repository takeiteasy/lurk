#include "wee.h"

struct weeeeeeeeeeeeeeeeeee {
    uint64_t test_a;
    uint64_t test_b;
};

static weeeeeeeeeeeeeeee* init(weeState* state) {
    printf("Initializing menu scene...\n");
    weeee *result = malloc(sizeof(struct weeeeeeeeeee));
    result->test_a = weeFindTexture(state, "test.png");
    result->test_b = weeFindTexture(state, "test2.png");
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
    weeSetPosition(state, 0.f, 0.f);
    
    weePushTexture(state, scene->test_a);
    weeBeginBatch(state);
    for (int i = 0; i < 10; i++) {
        weePositionMoveBy(state, 5.f, 5.f);
        weeDrawTextureBatch(state);
    }
    weeEndBatch(state);
    weePopTexture(state);
    
    weeSetPosition(state, 100.f, 100.f);
    weePushTexture(state, scene->test_b);
    weeDrawTexture(state);
    weePopTexture(state);
}

EXPORT const weeScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};

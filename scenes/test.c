#include "gamekit.h"

struct GameKit {
    uint64_t test_a;
    gkEntity test_b;
    gkEntity test_c;
};

typedef struct {
    int dummy;
} TestComponent;

static GameKit* init(gkState* state) {
    GameKit *result = malloc(sizeof(struct GameKit));
    result->test_a = gkFindTexture(state, "test2.png");
    result->test_b = gkNewEntity(state);
    result->test_c = GK_COMPONENT(state, TestComponent);
    gkAttach(state, result->test_b, result->test_c);
    return result;
}

static void deinit(gkState* state, GameKit *scene) {
    free(scene);
}

static void reload(gkState* state, GameKit *scene) {

}

static void unload(gkState* state, GameKit *scene) {

}

static void event(gkState* state, GameKit *scene, gkEventType event) {

}

static void frame(gkState* state, GameKit *scene, float delta) {
    int width, height;
    gkWindowSize(state, &width, &height);
    float ratio = (float)width / (float)height;
    gkViewport(state, 0, 0, width, height);
    gkProject(state, -ratio, ratio, 1.f, -1.f);
    gkSetColor(state, 1.f, 0.f, 0.f, 1.f);
    gkClear(state);

    assert(gkHas(state, scene->test_b, scene->test_c));

    gkSetColor(state, 0.f, 0.f, 0.f, 1.f);
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

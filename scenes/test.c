#include "lurk.h"

struct lurkContext {
    uint64_t test_a;
    lurkEntity test_b;
    lurkEntity test_c;
};

typedef struct {
    int dummy;
} TestComponent;

static lurkContext* init(lurkState* state) {
    lurkContext *result = malloc(sizeof(struct lurkContext));
    result->test_a = lurkFindTexture(state, "test2.png");
    result->test_b = lurkNewEntity(state);
    result->test_c = LURK_COMPONENT(state, TestComponent);
    lurkAttach(state, result->test_b, result->test_c);
    return result;
}

static void deinit(lurkState* state, lurkContext *context) {
    free(context);
}

static void reload(lurkState* state, lurkContext *context) {

}

static void unload(lurkState* state, lurkContext *context) {

}

static void event(lurkState* state, lurkContext *context, lurkEventType event) {

}

static void frame(lurkState* state, lurkContext *context, float delta) {
    int width, height;
    lurkWindowSize(state, &width, &height);
    float ratio = (float)width / (float)height;
    lurkViewport(state, 0, 0, width, height);
    lurkProject(state, -ratio, ratio, 1.f, -1.f);
    lurkSetColor(state, 1.f, 0.f, 0.f, 1.f);
    lurkClear(state);

    assert(lurkHas(state, context->test_b, context->test_c));

    lurkSetColor(state, 0.f, 0.f, 0.f, 1.f);
    lurkDrawFilledRect(state, -.5f, -.5f, 1.f, 1.f);
}

EXPORT const lurkScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};

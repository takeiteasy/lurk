#include "lurk.h"

struct lurkContext {
    uint64_t texture;
};

typedef struct {
    int dummy;
} TestComponent;

static lurkContext* init(lurkState* state) {
    lurkContext *result = malloc(sizeof(struct lurkContext));
    result->texture = lurkFindTexture(state, "test2.png");
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

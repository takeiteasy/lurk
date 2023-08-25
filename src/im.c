//
//  im.c
//  wee
//
//  Created by George Watson on 25/08/2023.
//

#include "wee.h"

typedef enum {
    INVALID_COMMAND = 0,
    VIEWPORT_COMMAND,
    SCISSOR_RECT_COMMAND,
    DRAW_CALL_COMMAND
} CommandType;

typedef struct {
    int x, y, w, h;
} Viewport;

typedef Viewport Scissor;

static StackEntry* CreateEntry(int id, void *data, StackEntry *next, StackEntry *prev) {
    assert(data);
    StackEntry *entry = malloc(sizeof(StackEntry));
    entry->data = data;
    entry->next = next;
    entry->prev = prev;
    return entry;
}

static void StackPush(Stack *stack, int id, void *data) {
    StackEntry *entry = CreateEntry(id, data, stack->front, NULL);
    if (stack->front)
        stack->front->prev = entry;
    stack->front = entry;
    if (!stack->back)
        stack->back = stack->front;
}

static void StackAppend(Stack *stack, int id, void *data) {
    StackEntry *entry = CreateEntry(id, data, NULL, NULL);
    if (!stack->back)
        stack->front = stack->back = entry;
    else {
        entry->prev = stack->back;
        stack->back->next = entry;
        stack->back = entry;
    }
}

static StackEntry* StackShift(Stack *stack) {
    if (!stack->front)
        return NULL;
    StackEntry *tmp = stack->front;
    stack->front = stack->front->next;
    if (stack->front)
        stack->front->prev = NULL;
    else
        stack->back = NULL;
    return tmp;
}

static StackEntry* StackDrop(Stack *stack) {
    if (!stack->back)
        return NULL;
    StackEntry *tmp = stack->back;
    stack->back = stack->back->prev;
    if (stack->back)
        stack->back->next = NULL;
    else
        stack->front = NULL;
    return tmp;
}

static sg_color ColorToSokol(Color col) {
    return (sg_color) {
        (float)col.r / 255.f,
        (float)col.g / 255.f,
        (float)col.b / 255.f,
        (float)col.a / 255.f
    };
}

void weeClearColor(weeState *state, Color color) {
    state->pass_action.colors[0].value = ColorToSokol(color);
}

void weeViewport(weeState *state, int x, int y, int width, int height) {
    Viewport *viewport = malloc(sizeof(Viewport));
    viewport->x = x;
    viewport->y = y;
    viewport->w = width;
    viewport->h = height;
    StackAppend(&state->commandStack, VIEWPORT_COMMAND, (void*)viewport);
}

void weeScissorRect(weeState *state, int x, int y, int width, int height) {
    Scissor *scissor = malloc(sizeof(Scissor));
    scissor->x = x;
    scissor->y = y;
    scissor->w = width;
    scissor->h = height;
    StackAppend(&state->commandStack, SCISSOR_RECT_COMMAND, (void*)scissor);
}

int weeCreateFramebuffer(weeState *state, int w, int h) {
    return 0;
}

void weeDestroyFramebuffer(weeState *state, int id) {

}

void weeEnableFramebuffer(weeState *state, int id) {

}

void weePushPipeline(weeState *state, sg_pipeline pip) {

}

void weePopPipeline(weeState *state) {

}

void weeBlendMode(weeState *state, int mode) {

}

void weeBeginPass(weeState *state) {

}

void weeDraw(weeState *state, int baseElement, int elementCount, int instanceCount) {

}

void weeEndPass(weeState *state) {

}

void weeCommit(weeState *state) {
    StackEntry *entry = NULL;
    while ((entry = StackDrop(&state->commandStack))) {
        switch (entry->id) {
            case INVALID_COMMAND:
                abort();
            case VIEWPORT_COMMAND:
            case SCISSOR_RECT_COMMAND:
                // TODO: Process viewport + scissor rect commands here
                break;
            case DRAW_CALL_COMMAND:
                // TODO: Process + free draw calls here
                break;
        }
        free(entry->data);
        free(entry);
    }
}


#include "wee.h"

struct weeeeeeeeeeeeeeeeeee {
    int test;
};

static weeeeeeeeeeeeeeee* init(void) {
    weeee *result = malloc(sizeof(struct weeeeeeeeeee));
    result->test = 1;
    return result;
}

static void deinit(weeeeeeeeeeeeeee *scene) {
    
}

static void reload(weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee *scene) {
    
}

static void unload(weeeeeeee *scene) {
}

static bool event(weeeeeeeeeeeeeeeeeeeeee *scene, weeEvent *e) {
    switch (e->type) {
        case KEYBOARD_EVENT:
            printf("EVENT: Keyboard key %d is now %s\n", e->Keyboard.key, e->Keyboard.isdown ? "pressed" : "released");
            break;
        case MOUSE_BUTTON_EVENT:
            printf("EVENT: Mouse button %d is now %s\n", e->Mouse.button, e->Mouse.isdown ? "pressed" : "released");
            break;
        case MOUSE_MOVE_EVENT:
            printf("EVENT: Mouse moved by %fx%f to %dx%d\n", e->Mouse.Position.dx, e->Mouse.Position.dy, e->Mouse.Position.x, e->Mouse.Position.y);
            break;
        case MOUSE_SCROLL_EVENT:
            printf("EVENT: Mouse scrolled by %fx%f\n", e->Mouse.Wheel.dx, e->Mouse.Wheel.dy);
            break;
        case WINDOW_RESIZED_EVENT:
            printf("EVENT: Window has resized to %dx%d\n", e->Window.Size.width, e->Window.Size.height);
            break;
        case WINDOW_FOCUS_EVENT:
            printf("EVENT: Window is now %s\n", e->Window.focused ? "focused" : "blured");
            break;
        case WINDOW_CLOSED_EVENT:
            printf("EVENT: Window closed\n");
            return false;
    }
    return true;
}

static bool frame(weeeeeeeeeeeeeee *scene, float delta) {
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

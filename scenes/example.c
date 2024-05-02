#include "lurk.h"

/* NOTE:
  A struct name `lurkContext` must be declared at the top of each scene. This acts as your scene's global state.
  The struct can't be typedef'd here, it is already pre-defined in `lurk.h`. */
struct lurkContext {
    int test;
};


/* NOTE:
   An `init` function is required for each scene. The init function must return an allocated `lurkContext` object */
static lurkContext* init(lurkState *state) {
    printf("Initializing the scene...\n");

    lurkContext *result = malloc(sizeof(struct lurkContext));
    return result;
}

/* INFO:
   `deinit` is called when the scene has been permenantly unloaded, like on exit or if scene has been swapped */
static void deinit(lurkState *state, lurkContext *context) {
    printf("Goodbye from scene!\n");
    free(context);
}

/* INFO:
   `reload` is called when the scene has been reloaded (post) due to modifications */
static void reload(lurkState *state, lurkContext *context) {
    printf("Scene has been reloaded\n");
}

/* INFO:
   `unload` is called when the scene has been reloaded (pre) due to modifications */
static void unload(lurkState *state, lurkContext *context) {
    printf("Scene has been unloaded!\n");
}

/* INFO:
   `event` is called when a window event is generated */
static void event(lurkState *state, lurkContext *context, lurkEventType event) {
    printf("Wow! Some sort of event!\n");
}

/* INFO:
   `frame` is, as the name suggests, called every frame. All your rendering code should probably go here. */
static void frame(lurkState *state, lurkContext *context, float delta) {
    // Put your rendering code in here ...
}

/* NOTE:
   Each scene must declare a function map. This is so the main program can find the address to the function in the library.
   It must be as shown below. It must be called `scene`, otherwise the main program will not be able to find it.
 */
EXPORT const lurkScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};

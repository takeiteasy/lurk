#include "gamekit.h"

/* NOTE:
  A struct name `GameKit` must be declared at the top of each scene. This acts as your scene's global state.
  The struct can't be typedef'd here, it is already pre-defined in `gamekit.h`. */
struct GameKit {
    int test;
};


/* NOTE:
   An `init` function is required for each scene. The init function must return an allocated `GameKit` object */
static GameKit* init(gkState *state) {
    printf("Initializing the scene...\n");

    GameKit *result = malloc(sizeof(struct GameKit));
    return result;
}

/* INFO:
   `deinit` is called when the scene has been permenantly unloaded, like on exit or if scene has been swapped */
static void deinit(gkState *state, GameKit *scene) {
    printf("Goodbye from scene!\n");
    free(scene);
}

/* INFO:
   `reload` is called when the scene has been reloaded (post) due to modifications */
static void reload(gkState *state, GameKit *scene) {
    printf("Scene has been reloaded\n");
}

/* INFO:
   `unload` is called when the scene has been reloaded (pre) due to modifications */
static void unload(gkState *state, GameKit *scene) {
    printf("Scene has been unloaded!\n");
}

/* INFO:
   `event` is called when a window event is generated */
static void event(gkState *state, GameKit *scene, gkEventType event) {
    printf("Wow! Some sort of event!\n");
}

/* INFO:
   `frame` is, as the name suggests, called every frame. All your rendering code should probably go here. */
static void frame(gkState *state, GameKit *scene, float delta) {
    // Put your rendering code in here ...
}

/* NOTE:
   Each scene must declare a function map. This is so the main program can find the address to the function in the library.
   It must be as shown below. It must be called `scene`, otherwise the main program will not be able to find it.
 */
EXPORT const gkScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};

//
//  wee.c
//  wee
//
//  Created by George Watson on 21/07/2023.
//

#define SOKOL_IMPL
#define MJSON_IMPLEMENTATION
#define JIM_IMPLEMENTATION
#define HASHMAP_IMPL
#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"
#include "sokol_args.h"
#include "sokol_time.h"
#include "jim.h"
#include "mjson.h"
#include "hashmap.h"
#include "wee.h"
#include "framebuffer.glsl.h"

typedef struct wis {
    const char *path;
    void *handle;
#if defined(WEE_POSIX)
    ino_t handleID;
#else
    FILETIME writeTime;
#endif
    weeeeeeeeeeeeeeeeeeeeeeeeeeeee *context;
    weeScene *scene;
    struct wis *next;
} weeInternalScene;

static struct {
    weeInternalScene *wis;
    struct hashmap *map;
    
    void *userdata;
    bool running;
    bool mouseHidden;
    bool mouseLocked;
    sapp_desc desc;
    
    sg_pass_action pass_action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
    sg_image color, depth;
} state = {
    .userdata = NULL,
    .running = false,
    .mouseHidden = false,
    .mouseLocked = false,
    .desc = (sapp_desc) {
#define X(NAME, TYPE, VAL, DEFAULT, DOCS) .VAL = DEFAULT,
        SETTINGS
#undef X
        .window_title = DEFAULT_WINDOW_TITLE
    },
    .pass_action = {
        .colors[0] = { .action=SG_ACTION_CLEAR, .value={0.f, 0.f, 0.f, 1.f} }
    }
};

// MARK: Config/Argument parsing function

static void Usage(const char *name) {
    printf("  usage: ./%s [options]\n\n  options:\n", name);
    printf("\t  help (flag) -- Show this message\n");
    printf("\t  config (string) -- Path to .json config file\n");
#define X(NAME, TYPE, VAL, DEFAULT, DOCS) \
    printf("\t  %s (%s) -- %s (default: %d)\n", NAME, #TYPE, DOCS, DEFAULT);
    SETTINGS
#undef X
}

static int LoadConfig(const char *path) {
    const char *data = NULL;
    if (!(data = LoadFile(path, NULL)))
        return 0;

    const struct json_attr_t config_attr[] = {
#define X(NAME, TYPE, VAL, DEFAULT,DOCS) \
        {(char*)#NAME, t_##TYPE, .addr.TYPE=&state.desc.VAL},
        SETTINGS
#undef X
        {NULL}
    };
    int status = json_read_object(data, config_attr, NULL);
    if (!status)
        return 0;
    free((void*)data);
    return 1;
}

#define jim_boolean jim_bool

static int ExportConfig(const char *path) {
    FILE *fh = fopen(path, "w");
    if (!fh)
        return 0;
    Jim jim = {
        .sink = fh,
        .write = (Jim_Write)fwrite
    };
    jim_object_begin(&jim);
#define X(NAME, TYPE, VAL, DEFAULT, DOCS) \
    jim_member_key(&jim, NAME);           \
    jim_##TYPE(&jim, state.desc.VAL);
    SETTINGS
#undef X
    jim_object_end(&jim);
    fclose(fh);
    return 1;
}

static int ParseArguments(int argc, char *argv[]) {
    const char *name = argv[0];
    sargs_desc desc = (sargs_desc) {
        .argc = argc - 1,
        .argv = (char**)(argv + 1)
    };
    sargs_setup(&desc);
    
#if !defined(WEE_EMSCRIPTEN)
    if (sargs_exists("help")) {
        Usage(name);
        return 0;
    }
    if (sargs_exists("config")) {
        const char *path = sargs_value("config");
        if (!path) {
            fprintf(stderr, "[ARGUMENT ERROR] No value passed for \"config\"\n");
            Usage(name);
            return 0;
        }
        if (!DoesFileExist(path)) {
            fprintf(stderr, "[FILE ERROR] No file exists at \"%s\"\n", path);
            Usage(name);
            return 0;
        }
        LoadConfig(path);
    }
#endif // WEE_EMSCRIPTEN
    
#define boolean 1
#define integer 0
#define X(NAME, TYPE, VAL, DEFAULT, DOCS)                                               \
    if (sargs_exists(NAME))                                                             \
    {                                                                                   \
        const char *tmp = sargs_value_def(NAME, #DEFAULT);                              \
        if (!tmp)                                                                       \
        {                                                                               \
            fprintf(stderr, "[ARGUMENT ERROR] No value passed for \"%s\"\n", NAME);     \
            Usage(name);                                                                \
            return 0;                                                                   \
        }                                                                               \
        if (TYPE == 1)                                                                  \
            state.desc.VAL = (int)atoi(tmp);                                            \
        else                                                                            \
            state.desc.VAL = sargs_boolean(NAME);                                       \
    }
    SETTINGS
#undef X
#undef boolean
#undef integer
    return 1;
}

// MARK: Window functions

int weeWindowWidth(void) {
    return state.running ? sapp_width() : -1;
}

int weeWindowHeight(void) {
    return state.running ? sapp_height() : -1;
}

int weeIsWindowFullscreen(void) {
    return state.running ? sapp_is_fullscreen() : state.desc.fullscreen;
}

void weeToggleFullscreen(void) {
    if (!state.running)
        state.desc.fullscreen = !state.desc.fullscreen;
    else
        sapp_toggle_fullscreen();
}

int weeIsCursorVisible(void) {
    return state.running ? sapp_mouse_shown() : state.mouseHidden;
}

void weeToggleCursorVisible(void) {
    if (state.running)
        sapp_show_mouse(!state.mouseHidden);
    state.mouseHidden = !state.mouseHidden;
}

int weeIsCursorLocked(void) {
    return state.running? sapp_mouse_locked() : state.mouseLocked;
}

void weeToggleCursorLock(void) {
    if (state.running)
        sapp_lock_mouse(!state.mouseLocked);
    state.mouseLocked = !state.mouseLocked;
}

// MARK: Program loop

static void InitCallback(void) {
    if (state.mouseHidden)
        sapp_show_mouse(false);
    if (state.mouseLocked)
        sapp_lock_mouse(true);
    
    sg_desc desc = (sg_desc) {
        // TODO: Add more configuration options for sg_desc
        .context = sapp_sgcontext()
    };
    sg_setup(&desc);
    stm_setup();
    
    sg_image_desc img_desc = {
        .width = sapp_width(),
        .height = sapp_height(),
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .sample_count = state.desc.sample_count,
        .render_target = true,
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE
    };
    state.color = sg_make_image(&img_desc);
    img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    state.depth = sg_make_image(&img_desc);
    sg_pass_desc pass_desc = (sg_pass_desc) {
        .color_attachments[0].image = state.color,
        .depth_stencil_attachment.image = state.depth
    };
    state.pass = sg_make_pass(&pass_desc);
    
    const float vertices[] = {
        // pos      // uv
        -1.f,  1.f, 0.f, 1.f,
         1.f,  1.f, 1.f, 1.f,
         1.f, -1.f, 1.f, 0.f,
        -1.f, -1.f, 0.f, 0.f,
    };
    const uint16_t indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    sg_buffer_desc vbuf_desc = (sg_buffer_desc) {
        .data = SG_RANGE(vertices)
    };
    sg_buffer_desc ibuf_desc = (sg_buffer_desc) {
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = SG_RANGE(indices),
    };
    state.bind = (sg_bindings) {
        .vertex_buffers[0] =  sg_make_buffer(&vbuf_desc),
        .index_buffer = sg_make_buffer(&ibuf_desc),
        .fs_images = state.color
    };
    
    sg_pipeline_desc pip_desc = (sg_pipeline_desc){
        .shader = sg_make_shader(framebuffer_program_shader_desc(sg_query_backend())),
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLES,
        .index_type = SG_INDEXTYPE_UINT16,
        .layout = {
            .attrs = {
                [ATTR_framebuffer_vs_position].format = SG_VERTEXFORMAT_FLOAT2,
                [ATTR_framebuffer_vs_texcoord].format = SG_VERTEXFORMAT_FLOAT2,
            }
        },
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
        .cull_mode = SG_CULLMODE_BACK,
    };
    state.pip = sg_make_pipeline(&pip_desc);
    
    weePushScene(WEE_FIRST_SCENE);
}

static char* RemoveExt(char* path) {
    char *ret = malloc(strlen(path) + 1);
    if (!ret)
        return NULL;
    strcpy(ret, path);
    char *ext = strrchr(ret, '.');
    if (ext)
        *ext = '\0';
    return ret;
}

#if defined(PP_WINDOWS)
static FILETIME Win32GetLastWriteTime(char* path) {
    FILETIME time;
    WIN32_FILE_ATTRIBUTE_DATA data;

    if (GetFileAttributesEx(path, GetFileExInfoStandard, &data))
        time = data.ftLastWriteTime;

    return time;
}
#endif

static bool ShouldReloadLibrary(weeInternalScene *wis) {
#if defined(PP_WINDOWS)
    FILETIME newTime = Win32GetLastWriteTime(Args.path);
    bool result = CompareFileTime(&newTime, &wis->writeTime);
    if (result)
        wis->writeTime = newTime;
    return result;
#else
    struct stat attr;
    bool result = !stat(wis->path, &attr) && wis->handleID != attr.st_ino;
    if (result)
        wis->handleID = attr.st_ino;
    return result;
#endif
}

static bool ReloadLibrary(weeInternalScene *wis) {
    assert(wis);
    if (!ShouldReloadLibrary(wis))
        return true;
    
    if (wis->handle) {
        if (wis->scene->unload)
            wis->scene->unload(wis->context);
        dlclose(wis->handle);
    }
    
#if defined(PP_WINDOWS)
    size_t newPathSize = strlen(wis->path) + 4;
    char *newPath = malloc(sizeof(char) * newPathSize);
    char *noExt = RemoveExt(wis->path);
    sprintf(newPath, "%s.tmp.dll", noExt);
    CopyFile(wis->path, newPath, 0);
    if (!(wis->handle = dlopen(newPath, RTLD_NOW)))
        goto BAIL;
    free(newPath);
    free(noExt);
    if (!wis->handle)
#else
    if (!(wis->handle = dlopen(wis->path, RTLD_NOW)))
#endif
        goto BAIL;
    if (!(wis->scene = dlsym(wis->handle, "scene")))
        goto BAIL;
    if (!wis->context) {
        if (!(wis->context = wis->scene->init()))
            goto BAIL;
    } else {
        if (wis->scene->reload)
            wis->scene->reload(wis->context);
    }
    return true;

BAIL:
    if (wis->handle)
        dlclose(wis->handle);
    wis->handle = NULL;
#if defined(PP_WINDOWS)
    memset(&writeTime, 0, sizeof(FILETIME));
#else
    wis->handleID = 0;
#endif
    return false;
}

static void FrameCallback(void) {
    const int width = sapp_width();
    const int height = sapp_height();
    const float delta = (float)sapp_frame_duration() * 60.f;
    
    if (state.wis)
        assert(ReloadLibrary(state.wis));
    
    sg_begin_pass(state.pass, &state.pass_action);
    if (state.wis && state.wis->scene->frame)
        state.wis->scene->frame(state.wis->context, delta);
    sg_end_pass();
    
    sg_begin_default_pass(&state.pass_action, width, height);
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 6, 1);
    sg_end_pass();
    
    sg_commit();
}

#define ForwardEvent(...)                     \
    if (state.wis && state.wis->scene->event) \
        state.wis->scene->event(state.wis->context, &((weeEvent)__VA_ARGS__))

static void EventCallback(const sapp_event* e) {
    switch (e->type) {
        case SAPP_EVENTTYPE_KEY_UP:
        case SAPP_EVENTTYPE_KEY_DOWN:
            ForwardEvent({
                .type = KEYBOARD_EVENT,
                .Keyboard.isdown = e->type == SAPP_EVENTTYPE_KEY_DOWN,
                .Keyboard.key = e->key_code
            });
            break;
        case SAPP_EVENTTYPE_MOUSE_UP:
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            ForwardEvent({
                .type = MOUSE_BUTTON_EVENT,
                .Mouse.isdown = e->type == SAPP_EVENTTYPE_KEY_DOWN,
                .Mouse.button = e->mouse_button
            });
            break;
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            // TODO: Track if mouse in window first
            ForwardEvent({
                .type = MOUSE_MOVE_EVENT,
                .Mouse.Position = {
                    .x = e->mouse_x,
                    .y = e->mouse_y,
                    .dx = e->mouse_dx,
                    .dy = e->mouse_dy
                }
            });
            break;
        case SAPP_EVENTTYPE_MOUSE_SCROLL:
            ForwardEvent({
                .type = MOUSE_SCROLL_EVENT,
                .Mouse.Wheel.dx = e->scroll_x,
                .Mouse.Wheel.dy = e->scroll_y
            });
            break;
        case SAPP_EVENTTYPE_RESIZED:
            ForwardEvent({
                .type = WINDOW_RESIZED_EVENT,
                .Window.Size.width = sapp_width(),
                .Window.Size.height = sapp_height()
            });
            break;
        case SAPP_EVENTTYPE_FOCUSED:
        case SAPP_EVENTTYPE_UNFOCUSED:
            ForwardEvent({
                .type = WINDOW_FOCUS_EVENT,
                .Window.focused = e->type == SAPP_EVENTTYPE_FOCUSED
            });
            break;
        // TODO: Wrap other event types
        case SAPP_EVENTTYPE_INVALID:
        case SAPP_EVENTTYPE_CHAR:
        case SAPP_EVENTTYPE_MOUSE_ENTER:
        case SAPP_EVENTTYPE_MOUSE_LEAVE:
        case SAPP_EVENTTYPE_TOUCHES_BEGAN:
        case SAPP_EVENTTYPE_TOUCHES_MOVED:
        case SAPP_EVENTTYPE_TOUCHES_ENDED:
        case SAPP_EVENTTYPE_TOUCHES_CANCELLED:
        case SAPP_EVENTTYPE_ICONIFIED:
        case SAPP_EVENTTYPE_RESTORED:
        case SAPP_EVENTTYPE_SUSPENDED:
        case SAPP_EVENTTYPE_RESUMED:
        case SAPP_EVENTTYPE_QUIT_REQUESTED:
        case SAPP_EVENTTYPE_CLIPBOARD_PASTED:
        case SAPP_EVENTTYPE_FILES_DROPPED:
        default:
            break;
    }
}

typedef struct {
    const char *name;
    weeInternalScene *wis;
} SceneBucket;

static int CompareScene(const void *a, const void *b, void *udata) {
    const SceneBucket *ua = a;
    const SceneBucket *ub = b;
    return strcmp(ua->name, ub->name);
}

static uint64_t HashScene(const void *item, uint64_t seed0, uint64_t seed1) {
    const SceneBucket *b = item;
    return hashmap_sip(b->name, strlen(b->name), seed0, seed1);
}

static void FreeScene(void *item) {
    SceneBucket *b = item;
    if (b->wis->scene->deinit)
        b->wis->scene->deinit(b->wis->context);
}

void weeCreateScene(const char *name, const char *path) {
    SceneBucket search = {.name = name};
    SceneBucket *found = NULL;
    if (!(found = hashmap_get(state.map, (void*)&search))) {
        weeInternalScene *wis = malloc(sizeof(weeInternalScene));
        wis->path = path;
        wis->context = NULL;
        wis->scene = NULL;
        wis->handle = NULL;
        assert(ReloadLibrary(wis));
        search.wis = wis;
        hashmap_set(state.map, (void*)&search);
    }
}

void weePushScene(const char *name) {
    SceneBucket search = {.name = name};
    SceneBucket *found = hashmap_get(state.map, (void*)&search);
    assert(found);
    if (state.wis) {
        found->wis->next = state.wis;
        if (state.wis->scene->unload)
            state.wis->scene->unload(state.wis->context);
    }
    state.wis = found->wis;
    if (state.wis->scene->reload)
        state.wis->scene->reload(state.wis->context);
}

void weePopScene(void) {
    assert(state.wis);
    state.wis = state.wis->next;
}

void weeDestroyScene(const char *name) {
    SceneBucket search = {.name = name};
    SceneBucket *found = NULL;
    if ((found = hashmap_get(state.map, (void*)&search)))
        hashmap_delete(state.map, (void*)found);
}

static void CleanupCallback(void) {
    state.running = false;
    sg_destroy_pass(state.pass);
    sg_destroy_pipeline(state.pip);
    sg_destroy_image(state.color);
    sg_destroy_image(state.depth);
#define X(NAME) \
    weeDestroyScene(NAME);
WEE_SCENES
#undef X
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
#if defined(WEE_ENABLE_CONFIG)
#if !defined(WEE_CONFIG_PATH)
    const char *configPath = JoinPath(UserPath(), DEFAULT_CONFIG_NAME);
#else
    const char *configPath = ResolvePath(WEE_CONFIG_PATH);
#endif
    
    if (DoesFileExist(configPath)) {
        if (!LoadConfig(configPath)) {
            fprintf(stderr, "[IMPORT CONFIG ERROR] Failed to import config from \"%s\"\n", configPath);
            fprintf(stderr, "errno (%d): \"%s\"\n", errno, strerror(errno));
            goto EXPORT_CONFIG;
        }
    } else {
    EXPORT_CONFIG:
        if (!ExportConfig(configPath)) {
            fprintf(stderr, "[EXPORT CONFIG ERROR] Failed to export config to \"%s\"\n", configPath);
            fprintf(stderr, "errno (%d): \"%s\"\n", errno, strerror(errno));
            abort();
        }
    }
#endif
#if defined(WEE_ENABLE_ARGUMENTS)
    if (argc > 1)
        if (!ParseArguments(argc, argv)) {
            fprintf(stderr, "[PARSE ARGUMENTS ERROR] Failed to parse arguments\n");
            abort();
        }
#endif
    
    state.map = hashmap_new(sizeof(SceneBucket), 0, 0, 0, HashScene, CompareScene, FreeScene, NULL);
    
#if defined(WEE_MAC)
#define DYLIB_EXT ".dylib"
#elif defined(WEE_WINDOWS)
#define DYLIB_EXT ".dll"
#elif defined(WEE_LINUX)
#define DYLIB_EXT ".so"
#endif
    
#if !defined(WEE_DYLIB_PATH)
#define WEE_DYLIB_PATH "./"
#endif
    
#define X(NAME) \
    weeCreateScene(NAME, WEE_DYLIB_PATH NAME DYLIB_EXT);
WEE_SCENES
#undef X
    
    state.desc.init_cb = InitCallback;
    state.desc.frame_cb = FrameCallback;
    state.desc.event_cb = EventCallback;
    state.desc.cleanup_cb = CleanupCallback;
    return state.desc;
}

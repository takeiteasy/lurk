//
//  wee.c
//  wee
//
//  Created by George Watson on 21/07/2023.
//

#include "wee.h"
#if !defined(WEE_STATE)
#include "framebuffer.glsl.h"

weeState state = {
    .running = false,
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
#endif
static weeState *currentState = NULL;

void weeInit(weeState *state) {
    currentState = state;
}

typedef struct {
    const char *name;
    weeInternalScene *wis;
} SceneBucket;

#if defined(WEE_WINDOWS)
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

static FILETIME Win32GetLastWriteTime(char* path) {
    FILETIME time;
    WIN32_FILE_ATTRIBUTE_DATA data;

    if (GetFileAttributesEx(path, GetFileExInfoStandard, &data))
        time = data.ftLastWriteTime;

    return time;
}
#endif

static bool ShouldReloadLibrary(weeInternalScene *wis) {
#if defined(WEE_WINDOWS)
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
            wis->scene->unload(currentState, wis->context);
        dlclose(wis->handle);
    }
    
#if defined(WEE_WINDOWS)
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
        if (!(wis->context = wis->scene->init(currentState)))
            goto BAIL;
    } else {
        if (wis->scene->reload)
            wis->scene->reload(currentState, wis->context);
    }
    return true;

BAIL:
    if (wis->handle)
        dlclose(wis->handle);
    wis->handle = NULL;
#if defined(WEE_WINDOWS)
    memset(&writeTime, 0, sizeof(FILETIME));
#else
    wis->handleID = 0;
#endif
    return false;
}

#if !defined(WEE_STATE)
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

// MARK: Program loop

static void InitCallback(void) {
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
    
#if defined(WEE_MAC)
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    uint64_t frequency = info.denom;
    frequency *= 1000000000L;
    state.timerFrequency = frequency / info.numer;
#elif defined(WEE_WINDOW)
    LARGE_INTEGER frequency;
    if (!QueryPerformanceFrequency(&frequency))
        return 1000L;
    state.timerFrequency = frequency.QuadPart;
#else
    state.timerFrequency = 1000000000L;
#endif
    
#define UPDATE_RATE 60.0
    state.updateMultiplicity = 1;
    state.unlockFramerate = 1;
    state.desiredFrameTime = state.timerFrequency * UPDATE_RATE;
    state.fixedDeltaTime = 1.0 / UPDATE_RATE;
    int64_t time60hz = state.timerFrequency / 60;
    state.snapFrequencies[0] = time60hz;
    state.snapFrequencies[1] = time60hz*2;
    state.snapFrequencies[2] = time60hz*3;
    state.snapFrequencies[3] = time60hz*4;
    state.snapFrequencies[4] = (time60hz+1)/2;
    state.snapFrequencies[5] = (time60hz+2)/3;
    state.snapFrequencies[6] = (time60hz+3)/4;
    state.maxVsyncError = state.timerFrequency * .0002;
    for (int i = 0; i < 4; i++)
        state.timeAverager[i] = state.desiredFrameTime;
    state.resync = true;
    state.prevFrameTime = stm_now();
    state.frameAccumulator = 0;
    
    state.windowWidth = sapp_width();
    state.windowHeight = sapp_height();
    
    weePushScene(&state, WEE_FIRST_SCENE);
}

static void FrameCallback(void) {
    if (state.fullscreen != state.fullscreenLast) {
        sapp_toggle_fullscreen();
        state.fullscreenLast = state.fullscreen;
    }
    
    if (state.cursorVisible != state.cursorVisibleLast) {
        sapp_show_mouse(state.cursorVisible);
        state.cursorVisibleLast = state.cursorVisible;
    }
    
    if (state.cursorLocked != state.cursorLockedLast) {
        sapp_lock_mouse(state.cursorLocked);
        state.cursorLockedLast = state.cursorLocked;
    }
    
#if defined(WEE_DEBUG)
    if (state.wis)
        assert(ReloadLibrary(state.wis));
#endif
    
    int64_t current_frame_time = stm_now();
    int64_t delta_time = current_frame_time - state.prevFrameTime;
    state.prevFrameTime = current_frame_time;
    
    if (delta_time > state.desiredFrameTime * 8)
        delta_time = state.desiredFrameTime;
    if (delta_time < 0)
        delta_time = 0;
    
    for (int i = 0; i < 7; ++i)
        if (labs(delta_time - state.snapFrequencies[i]) < state.maxVsyncError) {
            delta_time = state.snapFrequencies[i];
            break;
        }
    
    for (int i = 0; i < 3; ++i)
        state.timeAverager[i] = state.timeAverager[i + 1];
    state.timeAverager[3] = delta_time;
    delta_time = 0;
    for (int i = 0; i < 4; ++i)
        delta_time += state.timeAverager[i];
    delta_time /= 4.f;
    
    if ((state.frameAccumulator += delta_time) > state.desiredFrameTime * 8)
        state.resync = true;
    
    if (state.resync) {
        state.frameAccumulator = 0;
        delta_time = state.desiredFrameTime;
        state.resync = false;
    }
    
    double render_time = 1.0;
    if (state.unlockFramerate) {
        int64_t consumedDeltaTime = delta_time;
        
        while (state.frameAccumulator >= state.desiredFrameTime) {
            if (state.wis && state.wis->scene->fixedupdate)
                state.wis->scene->fixedupdate(&state, state.wis->context, state.fixedDeltaTime);
            if (consumedDeltaTime > state.desiredFrameTime) {
                if (state.wis && state.wis->scene->update)
                    state.wis->scene->update(&state, state.wis->context, state.fixedDeltaTime);
                consumedDeltaTime -= state.desiredFrameTime;
            }
            state.frameAccumulator -= state.desiredFrameTime;
        }
        
        if (state.wis && state.wis->scene->update)
            state.wis->scene->update(&state, state.wis->context, (double)consumedDeltaTime / state.timerFrequency);
        render_time = (double)state.frameAccumulator / state.desiredFrameTime;
    } else {
        while (state.frameAccumulator >= state.desiredFrameTime*state.updateMultiplicity) {
            for (int i = 0; i < state.updateMultiplicity; ++i) {
                if (state.wis && state.wis->scene->fixedupdate)
                    state.wis->scene->fixedupdate(&state, state.wis->context, state.fixedDeltaTime);
                if (state.wis && state.wis->scene->update)
                    state.wis->scene->update(&state, state.wis->context, state.fixedDeltaTime);
                state.frameAccumulator -= state.desiredFrameTime;
            }
        }
    }
    
    sg_begin_pass(state.pass, &state.pass_action);
    if (state.wis && state.wis->scene->frame)
        state.wis->scene->frame(&state, state.wis->context, render_time);
    sg_end_pass();
    
    sg_begin_default_pass(&state.pass_action, state.windowWidth, state.windowHeight);
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 6, 1);
    sg_end_pass();
    
    sg_commit();
}

static void EventCallback(const sapp_event* e) {
    switch (e->type) {
        case SAPP_EVENTTYPE_RESIZED:
            state.windowWidth = e->window_width;
            state.windowHeight = e->window_height;
            break;
        default:
            break;
    }
    if (state.wis && state.wis->scene->event)
        state.wis->scene->event(&state, state.wis->context, e);
}

static void CleanupCallback(void) {
    state.running = false;
    sg_destroy_pass(state.pass);
    sg_destroy_pipeline(state.pip);
    sg_destroy_image(state.color);
    sg_destroy_image(state.depth);
#define X(NAME) \
    weeDestroyScene(&state, NAME);
WEE_SCENES
#undef X
    sg_shutdown();
}

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
        b->wis->scene->deinit(&state, b->wis->context);
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
#define WEE_DYLIB_PATH ResolvePath("./");
#endif
    
#define X(NAME) \
    weeCreateScene(&state, NAME, WEE_DYLIB_PATH NAME DYLIB_EXT);
WEE_SCENES
#undef X
    weeInit(&state);
    
    state.desc.init_cb = InitCallback;
    state.desc.frame_cb = FrameCallback;
    state.desc.event_cb = EventCallback;
    state.desc.cleanup_cb = CleanupCallback;
    return state.desc;
}
#endif

void weeCreateScene(weeState *state, const char *name, const char *path) {
    SceneBucket search = {.name = name};
    SceneBucket *found = NULL;
    if (!(found = hashmap_get(state->map, (void*)&search))) {
        weeInternalScene *wis = malloc(sizeof(weeInternalScene));
        wis->path = path;
        wis->context = NULL;
        wis->scene = NULL;
        wis->handle = NULL;
        assert(ReloadLibrary(wis));
        search.wis = wis;
        hashmap_set(state->map, (void*)&search);
    }
}

void weePushScene(weeState *state, const char *name) {
    SceneBucket search = {.name = name};
    SceneBucket *found = hashmap_get(state->map, (void*)&search);
    assert(found);
    bool reload = false;
    if (state->wis) {
        found->wis->next = state->wis;
        if (state->wis->scene->unload)
            state->wis->scene->unload(state, state->wis->context);
        reload = true;
    }
    state->wis = found->wis;
    if (reload && state->wis->scene->reload)
        state->wis->scene->reload(state, state->wis->context);
}

void weePopScene(weeState *state) {
    assert(state->wis);
    if (state->wis->scene->unload)
        state->wis->scene->unload(state, state->wis->context);
    state->wis = state->wis->next;
}

void weeDestroyScene(weeState *state, const char *name) {
    SceneBucket search = {.name = name};
    SceneBucket *found = NULL;
    if ((found = hashmap_get(state->map, (void*)&search)))
        hashmap_delete(state->map, (void*)found);
}

int weeWindowWidth(weeState *state) {
    return state->windowWidth;
}

int weeWindowHeight(weeState *state) {
    return state->windowHeight;
}

int weeIsWindowFullscreen(weeState *state) {
    return state->fullscreen;
}

void weeToggleFullscreen(weeState *state) {
    state->fullscreen = !state->fullscreen;
}

int weeIsCursorVisible(weeState *state) {
    return state->cursorVisible;
}

void weeToggleCursorVisible(weeState *state) {
    state->cursorVisible = !state->cursorVisible;
}

int weeIsCursorLocked(weeState *state) {
    return state->cursorLocked;
}

void weeToggleCursorLock(weeState *state) {
    state->cursorLocked = !state->cursorLocked;
}


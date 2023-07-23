//
//  wee.c
//  wee
//
//  Created by George Watson on 21/07/2023.
//

#include "wee.h"
#define SOKOL_IMPL
#include "sokol_gfx.h"
#define SOKOL_NO_ENTRY
#include "sokol_app.h"
#include "sokol_glue.h"
#include "sokol_args.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "sokol_imgui.h"
#include "framebuffer.glsl.h"
#include "sokol_time.h"
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#if defined(WEE_WINDOWS)
#include <io.h>
#define F_OK    0
#define access _access
#else
#include <unistd.h>
#endif

#if defined(WEE_WINDOWS)
#include <shlobj.h>
#if !defined(_DLL)
#include <shellapi.h>
#pragma comment(lib, "shell32")

extern int main(int argc, const char *argv[]);

#ifdef UNICODE
int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
#else
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif // UNICODE
{
    int n, argc;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    char **argv = calloc(argc + 1, sizeof(int));

    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    for (n = 0; n < argc; n++) {
        int len = WideCharToMultiByte(CP_UTF8, 0, wargv[n], -1, 0, 0, NULL, NULL);
        argv[n] = malloc(len);
        WideCharToMultiByte(CP_UTF8, 0, wargv[n], -1, argv[n], len, NULL, NULL);
    }
    return main(argc, argv);
}
#endif // !_DLL
#endif // WEE_WINDOWS

#if !defined(MAX_PATH)
#if defined(WEE_MAC)
#define MAX_PATH 255
#elif defined(WEE_WINDOWS)
#define MAX_PATH 256
#elif defined(WEE_LINUX)
#define MAX_PATH 4096
#endif
#endif

#if defined(WEE_POSIX)
#define PATH_SEPERATOR "/"
#else
#define PATH_SEPERATOR "\\"
#endif

#if !defined(DEFAULT_CONFIG_NAME)
#if defined(WEE_POSIX)
#define DEFAULT_CONFIG_NAME ".wee.json"
#else
#define DEFAULT_CONFIG_NAME "wee.json"
#endif // WEE_POSX
#endif // DEFAULT_CONFIG_NAME

#if !defined(DEFAULT_WINDOW_WIDTH)
#define DEFAULT_WINDOW_WIDTH 640
#endif

#if !defined(DEFAULT_WINDOW_HEIGHT)
#define DEFAULT_WINDOW_HEIGHT 480
#endif

#if !defined(DEFAULT_WINDOW_TITLE)
#define DEFAULT_WINDOW_TITLE "WEE"
#endif

#define SAFE_FREE(X)    \
    do                  \
    {                   \
        if ((X))        \
        {               \
            free((X));  \
            (X) = NULL; \
        }               \
    } while (0)

#if defined(DEBUG)
#define ASSERT(X) \
    do {                                                                                       \
        if (!(X)) {                                                                            \
            fprintf(stderr, "ERROR! Assertion hit! %s:%s:%d\n", __FILE__, __func__, __LINE__); \
            assert(false);                                                                     \
        }                                                                                      \
    } while(0)
#define ECS_ASSERT(X, Y, V)                                                                    \
    do {                                                                                       \
        if (!(X)) {                                                                            \
            fprintf(stderr, "ERROR! Assertion hit! %s:%s:%d\n", __FILE__, __func__, __LINE__); \
            Dump##Y(V);                                                                        \
            assert(false);                                                                     \
        }                                                                                      \
    } while(0)
#else
#define ECS_ASSERT(X, _, __) assert(X)
#define ASSERT(X) assert(X)
#endif

// MARK: Math functions

#define X(N)                                               \
    vec##N Vec##N##Zero(void)                              \
    {                                                      \
        vec##N v = {0};                                    \
        for (int i = 0; i < N; i++)                        \
            v[i] = 0;                                      \
        return v;                                          \
    }                                                      \
    static float Vec##N##Sum(vec##N v)                     \
    {                                                      \
        float r = 0.f;                                     \
        for (int i = 0; i < N; i++)                        \
            r += v[i];                                     \
        return r;                                          \
    }                                                      \
    float Vec##N##LengthSqr(vec##N v)                      \
    {                                                      \
        return Vec##N##Sum(v * v);                         \
    }                                                      \
    float Vec##N##Length(vec##N v)                         \
    {                                                      \
        return sqrtf(Vec##N##LengthSqr(v));                \
    }                                                      \
    float Vec##N##Dot(vec##N v1, vec##N v2)                \
    {                                                      \
        return Vec##N##Sum(v1 * v2);                       \
    }                                                      \
    vec##N Vec##N##Normalize(vec##N v)                     \
    {                                                      \
        vec##N r = {0};                                    \
        float l = Vec##N##Length(v);                       \
        for (int i = 0; i < N; i++)                        \
            r[i] = v[i] * (1.f / l);                       \
        return r;                                          \
    }
SIZES
#undef X

vec3 Vec3Cross(vec3 v1, vec3 v2) {
    return (vec3) { v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x };
}

#define SETTINGS                                                                             \
X("width", integer, width, DEFAULT_WINDOW_WIDTH, "Set window width")                         \
X("height", integer, height, DEFAULT_WINDOW_HEIGHT, "Set window height")                     \
X("sampleCount", integer, sample_count, 4, "Set the MSAA sample count of the   framebuffer") \
X("swapInterval", integer, swap_interval, 1, "Set the preferred swap interval")              \
X("highDPI", boolean, high_dpi, true, "Enable high-dpi compatability")                       \
X("fullscreen", boolean, fullscreen, false, "Set fullscreen")                                \
X("alpha", boolean, alpha, false, "Enable/disable alpha channel on framebuffers")            \
X("clipboard", boolean, enable_clipboard, false, "Enable clipboard support")                 \
X("clipboardSize", integer, clipboard_size, 1024, "Size of clipboard buffer (in bytes)")     \
X("drapAndDrop", boolean, enable_dragndrop, false, "Enable drag-and-drop files")             \
X("maxDroppedFiles", integer, max_dropped_files, 1, "Max number of dropped files")           \
X("maxDroppedFilesPathLength", integer, max_dropped_file_path_length, MAX_PATH, "Max path length for dropped files")

// MARK: Global state

typedef struct {
    Entity *sparse;
    Entity *dense;
    size_t sizeOfSparse;
    size_t sizeOfDense;
} EcsSparse;

typedef struct {
    Entity componentId;
    void *data;
    size_t sizeOfData;
    size_t sizeOfComponent;
    EcsSparse *sparse;
} EcsStorage;

static struct {
#define X(NAME, ARGS) void(*NAME##Callback)ARGS;
    WEE_CALLBACKS
#undef X
    const char *configPath;
    void *userdata;
    bool running;
    bool mouseHidden;
    bool mouseLocked;
    sapp_desc desc;
    
    struct {
        bool button_down[SAPP_MAX_KEYCODES];
        bool button_clicked[SAPP_MAX_KEYCODES];
        bool mouse_down[SAPP_MAX_MOUSEBUTTONS];
        bool mouse_clicked[SAPP_MAX_MOUSEBUTTONS];
        vec2 mouse_pos, last_mouse_pos;
        vec2 mouse_scroll_delta, mouse_delta;
    } input;
    
    sg_pass_action pass_action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
    sg_image color, depth;
    
    struct {
        EcsStorage **storages;
        size_t sizeOfStorages;
        Entity *entities;
        size_t sizeOfEntities;
        uint32_t *recyclable;
        size_t sizeOfRecyclable;
        uint32_t nextAvailableId;
    } ecs;
} state = {
#define X(NAME, ARGS) .NAME##Callback = NULL,
    WEE_CALLBACKS
#undef X
    .configPath = NULL,
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

#define weeCallCallback(CB, ...) \
    if (state.CB##Callback)        \
        state.CB##Callback(state.userdata, __VA_ARGS__)

// MARK: Config/Argument parsing function

#if defined(WEE_ENABLE_CONFIG)
#define MJSON_IMPL
#include "mjson.h"
#define JIM_IMPLEMENTATION
#include "jim.h"
#if defined(WEE_POSIX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#else
#include <io.h>
#define F_OK 0
#define access _access
#endif

static const char* UserPath(void) {
    const char *result;
    if (!(result = getenv("HOME"))) {
#if defined(WEE_POSIX)
        result = getpwuid(getuid())->pw_dir;
#else
        WCHAR profilePath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, profilePath))) {
            static char buffer[MAX_PATH];
            if (wcstombs(buffer, profilePath, sizeof(buffer)) == MAX_PATH)
                buffer[MAX_PATH-1] = '\0';
            return buffer;
        }
#endif
    }
    return result;
}

static const char* JoinPath(const char *a, const char *b) {
    static char buffer[MAX_PATH];
    buffer[0] = '\0';
    strcat(buffer, a);
    const char *seperator = PATH_SEPERATOR;
    if (a[strlen(a)-1] != seperator[0] &&
        b[0] != seperator[0])
        strcat(buffer, seperator);
    strcat(buffer, b);
    return buffer;
}

static const char* ConfigPath(void) {
#if defined(WEE_CONFIG_FILE) && !defined(WEE_CONFIG_PATH)
    return WEE_CONFIG_FILE
#else
#if defined(WEE_CONFIG_PATH)
    const char *path = WEE_CONFIG_PATH;
#else
    const char *path = UserPath();
#endif
#if defined(WEE_CONFIG_FILE)
    cosnt char *file = WEE_CONFIG_FILE;
#else
    const char *file = DEFAULT_CONFIG_NAME;
#endif
    return JoinPath(path, file);
#endif
}

static int FileExists(const char *path) {
    return !access(path, F_OK);
}

#if defined(WEE_ENABLE_ARGUMENTS)
static void Usage(const char *name) {
    printf("  usage: ./%s [options]\n\n  options:\n", name);
    printf("\t  help (flag) -- Show this message\n");
#if defined(WEE_ENABLE_CONFIG)
    printf("\t  config (string) -- Path to .json config file (default: %s)\n", ConfigPath());
#endif
#define X(NAME, TYPE, VAL, DEFAULT, DOCS) \
    printf("\t  %s (%s) -- %s (default: %d)\n", NAME, #TYPE, DOCS, DEFAULT);
    SETTINGS
#undef X
}
#endif // WEE_ENABLE_ARGUMENTS

static int LoadConfig(const char *path) {
#if defined(WEE_ENABLE_CONFIG)
    const char *data = NULL;
    if (!(data = LoadFile(data, NULL)))
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
#endif // WEE_ENABLE_CONFIG
    return 1;
}

#define jim_boolean jim_bool

static int ExportConfig(const char *path) {
#if defined(WEE_ENABLE_CONFIG)
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
#endif // WEE_ENABLE_CONFIG
    return 1;
}

static int ParseArguments(int argc, const char *argv[]) {
#if defined(WEE_ENABLE_ARGUMENTS)
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
#if defined(WEE_ENABLE_CONFIG)
    if (sargs_exists("config")) {
        const char *path = sargs_value("config");
        if (!path) {
            fprintf(stderr, "[ARGUMENT ERROR] No value passed for \"config\"\n");
            Usage(name);
            return 0;
        }
        if (!FileExists(path)) {
            fprintf(stderr, "[FILE ERROR] No file exists at \"%s\"\n", path);
            Usage(name);
            return 0;
        }
        LoadConfig(path);
    }
#endif // WEE_ENABLE_CONFIG
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
#endif // WEE_ENABLE_ARGUMENTS
    return 1;
}
#endif // WEE_ENABLE_CONFIG


#define X(NAME, ARGS) void(*NAME##Callback)ARGS,
void weeSetCallbacks(WEE_CALLBACKS void* userdata) {
#undef X
#define X(NAME, ARGS) \
    state.NAME##Callback = NAME##Callback;
    WEE_CALLBACKS
#undef X
    state.userdata = userdata;
}

#define X(NAME, ARGS)                                        \
    void weeSet##NAME##Callback(void(*NAME##Callback)ARGS) { \
        state.NAME##Callback = NAME##Callback;               \
    }
WEE_CALLBACKS
#undef X

// MARK: Loop functions

static void SokolInitCallback(void) {
    if (state.mouseHidden)
        sapp_show_mouse(false);
    if (state.mouseLocked)
        sapp_lock_mouse(true);
    
    sg_desc desc = (sg_desc) {
        // TODO: Add more configuration options for sg_desc
        .context = sapp_sgcontext()
    };
    sg_setup(&desc);
    
    simgui_desc_t imgui_desc = (simgui_desc_t){0};
    simgui_setup(&imgui_desc);
    
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
    
    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
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
    });

    
    state.running = true;
    if (state.InitCallback)
        state.InitCallback(state.userdata);
}

// TODO: Split frame callback into fixed and unfixed timestep

static void SokolFrameCallback(void) {
    const int width = sapp_width();
    const int height = sapp_height();
    
//    simgui_frame_desc_t frame = (simgui_frame_desc_t){
//        .width = width,
//        .height = height,
//        .delta_time = sapp_frame_duration(),
//        .dpi_scale = sapp_dpi_scale()
//    };
//    simgui_new_frame(&frame);
//    igText("Hello, world!");
    
    sg_begin_pass(state.pass, &state.pass_action);
    if (state.FrameCallback)
        state.FrameCallback(state.userdata, 0.f);
    sg_end_pass();
    
    sg_begin_default_pass(&state.pass_action, width, height);
    sg_apply_pipeline(state.pip);

    sg_apply_bindings(&state.bind);
    sg_draw(0, 6, 1);
//    simgui_render();
    sg_end_pass();
    sg_commit();
    
    state.input.mouse_delta = state.input.mouse_scroll_delta = Vec2Zero();
    for (int i = 0; i < SAPP_MAX_KEYCODES; i++)
        if (state.input.button_clicked[i])
            state.input.button_clicked[i] = false;
    for (int i = 0; i < SAPP_MAX_MOUSEBUTTONS; i++)
        if (state.input.mouse_clicked[i])
            state.input.mouse_clicked[i] = false;
}

static void SokolEventCallback(const sapp_event* e) {
    simgui_handle_event(e);
    switch (e->type) {
        case SAPP_EVENTTYPE_KEY_DOWN:
#if defined(DEBUG)
            if (e->modifiers & SAPP_MODIFIER_SUPER && e->key_code == SAPP_KEYCODE_W)
                sapp_quit();
#endif
            state.input.button_down[e->key_code] = true;
            break;
        case SAPP_EVENTTYPE_KEY_UP:
            state.input.button_down[e->key_code] = false;
            state.input.button_clicked[e->key_code] = true;
            break;
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            state.input.mouse_down[e->mouse_button] = true;
            break;
        case SAPP_EVENTTYPE_MOUSE_UP:
            state.input.mouse_down[e->mouse_button] = false;
            state.input.mouse_clicked[e->mouse_button] = true;
            break;
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            state.input.last_mouse_pos = state.input.mouse_pos;
            state.input.mouse_pos = (vec2){e->mouse_x, e->mouse_y};
            state.input.mouse_delta = (vec2){e->mouse_dx, e->mouse_dy};
            break;
        case SAPP_EVENTTYPE_MOUSE_SCROLL:
            state.input.mouse_scroll_delta = (vec2){e->scroll_x, e->scroll_y};
            break;
        case SAPP_EVENTTYPE_RESIZED:
            state.desc.width = e->window_width;
            state.desc.height = e->window_height;
            break;
        default:
            break;
    }
}

static void SokolCleanupCallback(void) {
    state.running = false;
    if (state.CleanupCallback)
        state.CleanupCallback(state.userdata);
    sg_destroy_pass(state.pass);
    sg_destroy_pipeline(state.pip);
    sg_destroy_image(state.color);
    sg_destroy_image(state.depth);
    simgui_shutdown();
    sg_shutdown();
}

static void UpdateTimer(Query *query) {
    Timer *timer = ECS_FIELD(query, Timer, 0);
    if (!timer->enabled)
        return;
    if (stm_ms(stm_since(timer->start)) > timer->interval) {
//        timer->cb(timer->userdata);
        timer->start = stm_now();
    }
}

int weeInit(int argc, const char *argv[]) {
    assert(!state.running); // TODO: Report error
    
#if defined(WEE_ENABLE_CONFIG)
    state.configPath = ConfigPath();
    if (FileExists(state.configPath)) {
        if (!LoadConfig(state.configPath)) {
            fprintf(stderr, "[IMPORT CONFIG ERROR] Failed to import config from \"%s\"\n", state.configPath);
            fprintf(stderr, "errno (%d): \"%s\"\n", errno, strerror(errno));
            goto EXPORT_CONFIG;
        }
    } else {
EXPORT_CONFIG:
        if (!ExportConfig(state.configPath)) {
            fprintf(stderr, "[EXPORT CONFIG ERROR] Failed to export config to \"%s\"\n", state.configPath);
            fprintf(stderr, "errno (%d): \"%s\"\n", errno, strerror(errno));
            return EXIT_FAILURE;
        }
    }
#endif
#if defined(WEE_ENABLE_ARGUMENTS)
    if (argc > 1)
        if (!ParseArguments(argc, argv)) {
            fprintf(stderr, "[PARSE ARGUMENTS ERROR] Failed to parse arguments\n");
            return EXIT_FAILURE;
        }
#endif
    
    state.desc.init_cb = SokolInitCallback;
    state.desc.frame_cb = SokolFrameCallback;
    state.desc.event_cb = SokolEventCallback;
    state.desc.cleanup_cb = SokolCleanupCallback;
    
    state.ecs.nextAvailableId = EcsNil;
    EcsSystem   = ECS_COMPONENT(System);
    EcsPrefab   = ECS_COMPONENT(Prefab);
    EcsRelation = ECS_COMPONENT(Relation);
    EcsChildOf  = ECS_TAG(result);
    EcsTimer    = ECS_COMPONENT(Timer);
    ECS_SYSTEM(UpdateTimer, EcsTimer);
    
    sapp_run(&state.desc);
    return 0;
}

// MARK: Event functions

bool IsKeyDown(weeKey key) {
    return state.input.button_down[key];
}

bool IsKeyUp(weeKey key) {
    return !state.input.button_down[key];
}

bool WasKeyClicked(weeKey key) {
    return state.input.button_clicked[key];
}

bool IsButtonDown(weeButton button) {
    return state.input.mouse_down[button];
}

bool IsButtonUp(weeButton button) {
    return !state.input.mouse_down[button];
}

bool WasButtonPressed(weeButton button) {
    return state.input.mouse_clicked[button];
}

// MARK: Window management functions

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

// MARK: ECS Function

#if defined(DEBUG)
static void DumpEntity(Entity e) {
    printf("(%llx: %d, %d, %d)\n", e.id, e.parts.id, e.parts.version, e.parts.flag);
}

static void DumpSparse(EcsSparse *sparse) {
    printf("*** DUMP SPARSE ***\n");
    printf("sizeOfSparse: %zu, sizeOfDense: %zu\n", sparse->sizeOfSparse, sparse->sizeOfDense);
    printf("Sparse Contents:\n");
    for (int i = 0; i < sparse->sizeOfSparse; i++)
        DumpEntity(sparse->sparse[i]);
    printf("Dense Contents:\n");
    for (int i = 0; i < sparse->sizeOfDense; i++)
        DumpEntity(sparse->dense[i]);
    printf("*** END SPARSE DUMP ***\n");
}

static void DumpStorage(EcsStorage *storage) {
    printf("*** DUMP STORAGE ***\n");
    printf("componentId: %u, sizeOfData: %zu, sizeOfComponent: %zu\n",
           storage->componentId.parts.id, storage->sizeOfData, storage->sizeOfComponent);
    DumpSparse(storage->sparse);
    printf("*** END STORAGE DUMP ***\n");
}
#else
static void DumpEntity(Entity e) {}
static void DumpSparse(EcsSparse *sparse) {}
static void DumpStorage(EcsStorage *storage) {}
#endif

#define ECS_COMPOSE_ENTITY(ID, VER, TAG) \
    (Entity)                             \
    {                                    \
        .parts = {                       \
            .id = ID,                    \
            .version = VER,              \
            .flag = TAG                  \
        }                                \
    }

Entity EcsSystem   = EcsNilEntity;
Entity EcsPrefab   = EcsNilEntity;
Entity EcsRelation = EcsNilEntity;
Entity EcsChildOf  = EcsNilEntity;
Entity EcsTimer    = EcsNilEntity;

static EcsSparse* NewSparse(void) {
    EcsSparse *result = malloc(sizeof(EcsSparse));
    *result = (EcsSparse){0};
    return result;
}

static bool SparseHas(EcsSparse *sparse, Entity e) {
    ASSERT(sparse);
    uint32_t id = ENTITY_ID(e);
    ASSERT(id != EcsNil);
    return (id < sparse->sizeOfSparse) && (ENTITY_ID(sparse->sparse[id]) != EcsNil);
}

static void SparseEmplace(EcsSparse *sparse, Entity e) {
    ASSERT(sparse);
    uint32_t id = ENTITY_ID(e);
    ASSERT(id != EcsNil);
    if (id >= sparse->sizeOfSparse) {
        const size_t newSize = id + 1;
        sparse->sparse = realloc(sparse->sparse, newSize * sizeof * sparse->sparse);
        for (size_t i = sparse->sizeOfSparse; i < newSize; i++)
            sparse->sparse[i] = EcsNilEntity;
        sparse->sizeOfSparse = newSize;
    }
    sparse->sparse[id] = (Entity) { .parts = { .id = (uint32_t)sparse->sizeOfDense } };
    sparse->dense = realloc(sparse->dense, (sparse->sizeOfDense + 1) * sizeof * sparse->dense);
    sparse->dense[sparse->sizeOfDense++] = e;
}

static size_t SparseRemove(EcsSparse *sparse, Entity e) {
    ASSERT(sparse);
    ECS_ASSERT(SparseHas(sparse, e), Sparse, sparse);
    
    const uint32_t id = ENTITY_ID(e);
    uint32_t pos = ENTITY_ID(sparse->sparse[id]);
    Entity other = sparse->dense[sparse->sizeOfDense-1];
    
    sparse->sparse[ENTITY_ID(other)] = (Entity) { .parts = { .id = pos } };
    sparse->dense[pos] = other;
    sparse->sparse[id] = EcsNilEntity;
    sparse->dense = realloc(sparse->dense, --sparse->sizeOfDense * sizeof * sparse->dense);
    
    return pos;
}

static size_t SparseAt(EcsSparse *sparse, Entity e) {
    ASSERT(sparse);
    uint32_t id = ENTITY_ID(e);
    ASSERT(id != EcsNil);
    return ENTITY_ID(sparse->sparse[id]);
}

static EcsStorage* NewStorage(Entity id, size_t sz) {
    EcsStorage *result = malloc(sizeof(EcsStorage));
    *result = (EcsStorage) {
        .componentId = id,
        .sizeOfComponent = sz,
        .sizeOfData = 0,
        .data = NULL,
        .sparse = NewSparse()
    };
    return result;
}

static bool StorageHas(EcsStorage *storage, Entity e) {
    ASSERT(storage);
    ASSERT(!ENTITY_IS_NIL(e));
    return SparseHas(storage->sparse, e);
}

static void* StorageEmplace(EcsStorage *storage, Entity e) {
    ASSERT(storage);
    storage->data = realloc(storage->data, (storage->sizeOfData + 1) * sizeof(char) * storage->sizeOfComponent);
    storage->sizeOfData++;
    void *result = &((char*)storage->data)[(storage->sizeOfData - 1) * sizeof(char) * storage->sizeOfComponent];
    SparseEmplace(storage->sparse, e);
    return result;
}

static void StorageRemove(EcsStorage *storage, Entity e) {
    ASSERT(storage);
    size_t pos = SparseRemove(storage->sparse, e);
    memmove(&((char*)storage->data)[pos * sizeof(char) * storage->sizeOfComponent],
            &((char*)storage->data)[(storage->sizeOfData - 1) * sizeof(char) * storage->sizeOfComponent],
            storage->sizeOfComponent);
    storage->data = realloc(storage->data, --storage->sizeOfData * sizeof(char) * storage->sizeOfComponent);
}

static void* StorageAt(EcsStorage *storage, size_t pos) {
    ASSERT(storage);
    ECS_ASSERT(pos < storage->sizeOfData, Storage, storage);
    return &((char*)storage->data)[pos * sizeof(char) * storage->sizeOfComponent];
}

static void* StorageGet(EcsStorage *storage, Entity e) {
    ASSERT(storage);
    ASSERT(!ENTITY_IS_NIL(e));
    return StorageAt(storage, SparseAt(storage->sparse, e));
}

bool EcsIsValid(Entity e) {
        uint32_t id = ENTITY_ID(e);
    return id < state.ecs.sizeOfEntities && ENTITY_CMP(state.ecs.entities[id], e);
}

static Entity EcsNewEntityType(uint8_t type) {
        if (state.ecs.sizeOfRecyclable) {
        uint32_t idx = state.ecs.recyclable[state.ecs.sizeOfRecyclable-1];
        Entity e = state.ecs.entities[idx];
        Entity new = ECS_COMPOSE_ENTITY(ENTITY_ID(e), ENTITY_VERSION(e), type);
        state.ecs.entities[idx] = new;
        state.ecs.recyclable = realloc(state.ecs.recyclable, --state.ecs.sizeOfRecyclable * sizeof(uint32_t));
        return new;
    } else {
        state.ecs.entities = realloc(state.ecs.entities, ++state.ecs.sizeOfEntities * sizeof(Entity));
        Entity e = ECS_COMPOSE_ENTITY((uint32_t)state.ecs.sizeOfEntities-1, 0, type);
        state.ecs.entities[state.ecs.sizeOfEntities-1] = e;
        return e;
    }
}

Entity EcsNewEntity(void) {
    return EcsNewEntityType(EcsEntityType);
}

static EcsStorage* EcsFind(Entity e) {
    for (int i = 0; i < state.ecs.sizeOfStorages; i++)
        if (ENTITY_ID(state.ecs.storages[i]->componentId) == ENTITY_ID(e))
            return state.ecs.storages[i];
    return NULL;
}

static EcsStorage* EcsAssure(Entity componentId, size_t sizeOfComponent) {
    EcsStorage *found = EcsFind(componentId);
    if (found)
        return found;
    EcsStorage *new = NewStorage(componentId, sizeOfComponent);
    state.ecs.storages = realloc(state.ecs.storages, (state.ecs.sizeOfStorages + 1) * sizeof * state.ecs.storages);
    state.ecs.storages[state.ecs.sizeOfStorages++] = new;
    return new;
}

bool EcsHas(Entity entity, Entity component) {
        ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(component), Entity, component);
    return StorageHas(EcsFind(component), entity);
}

Entity EcsNewComponent(size_t sizeOfComponent) {
    Entity e = EcsNewEntityType(EcsComponentType);
    return EcsAssure(e, sizeOfComponent) ? e : EcsNilEntity;
}

Entity EcsNewSystem(SystemCb fn, size_t sizeOfComponents, ...) {
    Entity e = EcsNewEntityType(EcsSystemType);
    EcsAttach(e, EcsSystem);
    System *c = EcsGet(e, EcsSystem);
    c->callback = fn;
    c->sizeOfComponents = sizeOfComponents;
    c->components = malloc(sizeof(Entity) * sizeOfComponents);
    c->enabled = true;
    
    va_list args;
    va_start(args, sizeOfComponents);
    for (int i = 0; i < sizeOfComponents; i++)
        c->components[i] = va_arg(args, Entity);
    va_end(args);
    return e;
}

Entity EcsNewPrefab(size_t sizeOfComponents, ...) {
    Entity e = EcsNewEntityType(EcsPrefabType);
    EcsAttach(e, EcsPrefab);
    Prefab *c = EcsGet(e, EcsPrefab);
    c->sizeOfComponents = sizeOfComponents;
    c->components = malloc(sizeof(Entity) * sizeOfComponents);
    
    va_list args;
    va_start(args, sizeOfComponents);
    for (int i = 0; i < sizeOfComponents; i++)
        c->components[i] = va_arg(args, Entity);
    va_end(args);
    return e;
}

Entity EcsNewTimer(int interval, bool enable, TimerCb cb, void *userdata) {
    Entity e = EcsNewEntityType(EcsTimerType);
    EcsAttach(e, EcsTimer);
    Timer *timer = EcsGet(e, EcsTimer);
    timer->start = stm_now();
    timer->enabled = enable;
    timer->interval = MAX(interval, 1);
    timer->cb = cb;
    timer->userdata = userdata;
    return e;
}
#define DEL_TYPES \
    X(System, 0)  \
    X(Prefab, 1)

void DestroyEntity(Entity e) {
        ECS_ASSERT(EcsIsValid(e), Entity, e);
    switch (e.parts.flag) {
#define X(TYPE, _)                                 \
        case Ecs##TYPE##Type: {                    \
            TYPE *s = EcsGet(e, Ecs##TYPE); \
            if (s && s->components)                \
                free(s->components);               \
            break;                                 \
        }
        DEL_TYPES
#undef X
    }
    for (size_t i = state.ecs.sizeOfStorages; i; --i)
        if (state.ecs.storages[i - 1] && SparseHas(state.ecs.storages[i - 1]->sparse, e))
            StorageRemove(state.ecs.storages[i - 1], e);
    uint32_t id = ENTITY_ID(e);
    state.ecs.entities[id] = ECS_COMPOSE_ENTITY(id, ENTITY_VERSION(e) + 1, 0);
    state.ecs.recyclable = realloc(state.ecs.recyclable, ++state.ecs.sizeOfRecyclable * sizeof(uint32_t));
    state.ecs.recyclable[state.ecs.sizeOfRecyclable-1] = id;
}

void EcsAttach(Entity entity, Entity component) {
    switch (component.parts.flag) {
        case EcsRelationType: // Use EcsRelation()
        case EcsSystemType: // NOTE: potentially could be used for some sort of event system
        case EcsTimerType:
            ASSERT(false);
        case EcsPrefabType: {
            Prefab *c = EcsGet(component, EcsPrefab);
            for (int i = 0; i < c->sizeOfComponents; i++) {
                if (ENTITY_IS_NIL(c->components[i]))
                    break;
                EcsAttach(entity, c->components[i]);
            }
            break;
        }
        case EcsComponentType:
        default: {
            ECS_ASSERT(EcsIsValid(entity), Entity, entity);
            ECS_ASSERT(EcsIsValid(component), Entity, component);
            EcsStorage *storage = EcsFind(component);
            ASSERT(storage);
            StorageEmplace(storage, entity);
            break;
        }
    }
}

void EcsAssociate(Entity entity, Entity object, Entity relation) {
        ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(object), Entity, object);
    ECS_ASSERT(ENTITY_ISA(object, Component), Entity, object);
    ECS_ASSERT(EcsIsValid(relation), Entity, relation);
    ECS_ASSERT(ENTITY_ISA(relation, Entity), Entity, relation);
    EcsAttach(entity, EcsRelation);
    Relation *pair = EcsGet(entity, EcsRelation);
    pair->object = object;
    pair->relation = relation;
}

void EcsDetach(Entity entity, Entity component) {
        ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(component), Entity, component);
    EcsStorage *storage = EcsFind(component);
    ASSERT(storage);
    ECS_ASSERT(StorageHas(storage, entity), Storage, storage);
    StorageRemove(storage, entity);
}

void EcsDisassociate(Entity entity) {
    ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsHas(entity, EcsRelation), Entity, entity);
    EcsDetach(entity, EcsRelation);
}

bool EcsHasRelation(Entity entity, Entity object) {
    ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(object), Entity, object);
    EcsStorage *storage = EcsFind(EcsRelation);
    if (!storage)
        return false;
    Relation *relation = StorageGet(storage, entity);
    if (!relation)
        return false;
    return ENTITY_CMP(relation->object, object);
}

bool EcsRelated(Entity entity, Entity relation) {
    ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(relation), Entity, relation);
    EcsStorage *storage = EcsFind(EcsRelation);
    if (!storage)
        return false;
    Relation *_relation = StorageGet(storage, entity);
    if (!_relation)
        return false;
    return ENTITY_CMP(_relation->relation, relation);
}

void* EcsGet(Entity entity, Entity component) {
        ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(component), Entity, component);
    EcsStorage *storage = EcsFind(component);
    ASSERT(storage);
    return StorageHas(storage, entity) ? StorageGet(storage, entity) : NULL;
}

void EcsSet(Entity entity, Entity component, const void *data) {
        ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(component), Entity, component);
    EcsStorage *storage = EcsFind(component);
    ASSERT(storage);
    
    void *componentData = StorageHas(storage, entity) ?
                                    StorageGet(storage, entity) :
                                    StorageEmplace(storage, entity);
    ASSERT(componentData);
    memcpy(componentData, data, storage->sizeOfComponent);
}

static void DestroyQuery(Query *query) {
    SAFE_FREE(query->componentIndex);
    SAFE_FREE(query->componentData);
}

void EcsRelations(Entity parent, Entity relation, void *userdata, SystemCb cb) {
    EcsStorage *pairs = EcsFind(EcsRelation);
    for (size_t i = 0; i < state.ecs.sizeOfEntities; i++) {
        Entity e = state.ecs.entities[i];
        if (!StorageHas(pairs, e))
            continue;
        Relation *pair = StorageGet(pairs, e);
        if (!ENTITY_CMP(pair->object, relation) || !ENTITY_CMP(pair->relation, parent))
            continue;
        Query query = {
            .entity = e,
            .componentData = malloc(sizeof(void*)),
            .componentIndex = malloc(sizeof(Entity)),
            .sizeOfComponentData = 1,
            .userdata = userdata
        };
        query.componentIndex[0] = relation;
        query.componentData[0] = (void*)pair;
        cb(&query);
        DestroyQuery(&query);
    }
}

void EcsEnableSystem(Entity system) {
    ECS_ASSERT(EcsIsValid(system), Entity, system);
    ECS_ASSERT(ENTITY_ISA(system, System), Entity, system);
    System *s = EcsGet(system, EcsSystem);
    s->enabled = true;
}

void EcsDisableSystem(Entity system) {
    ECS_ASSERT(EcsIsValid(system), Entity, system);
    ECS_ASSERT(ENTITY_ISA(system, System), Entity, system);
    System *s = EcsGet(system, EcsSystem);
    s->enabled = false;
}

void EcsEnableTimer(Entity timer) {
    ECS_ASSERT(EcsIsValid(timer), Entity, timer);
    ECS_ASSERT(ENTITY_ISA(timer, Timer), Entity, timer);
    Timer *t = EcsGet(timer, EcsTimer);
    t->enabled = true;
    t->start = stm_now();
}

void EcsDisableTimer(Entity timer) {
    ECS_ASSERT(EcsIsValid(timer), Entity, timer);
    ECS_ASSERT(ENTITY_ISA(timer, Timer), Entity, timer);
    Timer *t = EcsGet(timer, EcsTimer);
    t->enabled = false;
}

void EcsRunSystem(Entity e) {
    ECS_ASSERT(EcsIsValid(e), Entity, e);
    ECS_ASSERT(ENTITY_ISA(e, System), Entity, e);
    System *system = EcsGet(e, EcsSystem);
    EcsQuery(system->callback, NULL, system->components, system->sizeOfComponents);
}

void EcsStep(void) {
    EcsStorage *storage = state.ecs.storages[ENTITY_ID(EcsSystem)];
    for (int i = 0; i < storage->sparse->sizeOfDense; i++) {
        System *system = StorageGet(storage, storage->sparse->dense[i]);
        if (system->enabled)
            EcsQuery(system->callback, NULL, system->components, system->sizeOfComponents);
    }
}

void EcsQuery(SystemCb cb, void *userdata, Entity *components, size_t sizeOfComponents) {
    for (size_t e = 0; e < state.ecs.sizeOfEntities; e++) {
        bool hasComponents = true;
        Query query = {
            .componentData = NULL,
            .componentIndex = NULL,
            .sizeOfComponentData = 0,
            .entity = state.ecs.entities[e],
            .userdata = userdata
        };
        
        for (size_t i = 0; i < sizeOfComponents; i++) {
            EcsStorage *storage = EcsFind(components[i]);
            if (!StorageHas(storage, state.ecs.entities[e])) {
                hasComponents = false;
                break;
            }
            
            query.sizeOfComponentData++;
            query.componentData = realloc(query.componentData, query.sizeOfComponentData * sizeof(void*));
            query.componentIndex = realloc(query.componentIndex, query.sizeOfComponentData * sizeof(Entity));
            query.componentIndex[query.sizeOfComponentData-1] = components[i];
            query.componentData[query.sizeOfComponentData-1] = StorageGet(storage, state.ecs.entities[e]);
        }
        
        if (hasComponents)
            cb(&query);
        DestroyQuery(&query);
    }
}

void* EcsQueryField(Query *query, size_t index) {
    return index >= query->sizeOfComponentData || ENTITY_IS_NIL(query->componentIndex[index]) ? NULL : query->componentData[index];
}

// MARK: Image functions

int RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return ((unsigned int)a << 24) | ((unsigned int)r << 16) | ((unsigned int)g << 8) | b;
}

int RGB(unsigned char r, unsigned char g, unsigned char b) {
    return RGBA(r, g, b, 255);
}

int RGBA1(unsigned char c, unsigned char a) {
    return RGBA(c, c, c, a);
}

int RGB1(unsigned char c) {
    return RGB(c, c, c);
}

unsigned char Rgba(int c) {
    return (unsigned char)((c >> 16) & 0xFF);
}

unsigned char rGba(int c) {
    return (unsigned char)((c >>  8) & 0xFF);
}

unsigned char rgBa(int c) {
    return (unsigned char)(c & 0xFF);
}

unsigned char rgbA(int c) {
    return (unsigned char)((c >> 24) & 0xFF);
}

int rGBA(int c, unsigned char r) {
    return (c & ~0x00FF0000) | (r << 16);
}

int RgBA(int c, unsigned char g) {
    return (c & ~0x0000FF00) | (g << 8);
}

int RGbA(int c, unsigned char b) {
    return (c & ~0x000000FF) | b;
}

int RGBa(int c, unsigned char a) {
    return (c & ~0x00FF0000) | (a << 24);
}

bool InitImage(Image *img, unsigned int w, unsigned int h) {
    img->w = w;
    img->h = h;
    size_t sz = w * h * sizeof(unsigned int) + 1;
    img->buf = malloc(sz);
    memset(img->buf, 0, sz);
    return true;
}

void DestroyImage(Image *img) {
    if (img->buf)
        free(img->buf);
    memset(img, 0, sizeof(Image));
}

void FillImage(Image *img, int col) {
    for (int i = 0; i < img->w * img->h; ++i)
        img->buf[i] = col;
}

static inline void flood_fn(Image *img, int x, int y, int new, int old) {
    if (new == old || PGet(img, x, y) != old)
        return;
    
    int x1 = x;
    while (x1 < img->w && PGet(img, x1, y) == old) {
        PSet(img, x1, y, new);
        x1++;
    }
    
    x1 = x - 1;
    while (x1 >= 0 && PGet(img, x1, y) == old) {
        PSet(img, x1, y, new);
        x1--;
    }
    
    x1 = x;
    while (x1 < img->w && PGet(img, x1, y) == new) {
        if(y > 0 && PGet(img, x1, y - 1) == old)
            flood_fn(img, x1, y - 1, new, old);
        x1++;
    }
    
    x1 = x - 1;
    while(x1 >= 0 && PGet(img, x1, y) == new) {
        if(y > 0 && PGet(img, x1, y - 1) == old)
            flood_fn(img, x1, y - 1, new, old);
        x1--;
    }
    
    x1 = x;
    while(x1 < img->w && PGet(img, x1, y) == new) {
        if(y < img->h - 1 && PGet(img, x1, y + 1) == old)
            flood_fn(img, x1, y + 1, new, old);
        x1++;
    }
    
    x1 = x - 1;
    while(x1 >= 0 && PGet(img, x1, y) == new) {
        if(y < img->h - 1 && PGet(img, x1, y + 1) == old)
            flood_fn(img, x1, y + 1, new, old);
        x1--;
    }
}

void FloodImage(Image *img, int x, int y, int col) {
    if (x < 0 || y < 0 || x >= img->w || y >= img->h)
        return;
    flood_fn(img, x, y, col, PGet(img, x, y));
}

void ClearImage(Image *img) {
    memset(img->buf, 0, img->w * img->h * sizeof(int));
}

#define BLEND(c0, c1, a0, a1) (c0 * a0 / 255) + (c1 * a1 * (255 - a0) / 65025)

void BSet(Image *img, int x, int y, int c) {
    unsigned char a = rgbA(c);
    if (!a || x < 0 || y < 0 || x >= img->w || y >= img->h)
        return;
    if (a == 255) {
        PSet(img, x, y, c);
        return;
    }
    int *p = &img->buf[y * img->w + x];
    unsigned char _b = rgbA(*p);
    *p = (a == 255 || !img) ? c : RGBA(BLEND(Rgba(c), Rgba(*p), a, _b),
                                       BLEND(rGba(c), rGba(*p), a, _b),
                                       BLEND(rgBa(c), rgBa(*p), a, _b),
                                       a + (_b * (255 - a) >> 8));
}

void PSet(Image *img, int x, int y, int col) {
    if (x >= 0 && y >= 0 && x < img->w && y < img->h)
        img->buf[y * img->w + x] = col;
}

int PGet(Image *img, int x, int y) {
    return (x >= 0 && y >= 0 && x < img->w && y < img->h) ? img->buf[y * img->w + x] : 0;
}

bool PasteImage(Image *dst, Image *src, int x, int y) {
    int ox, oy, c;
    for (ox = 0; ox < src->w; ++ox) {
        for (oy = 0; oy < src->h; ++oy) {
            if (oy > dst->h)
                break;
            c = PGet(src, ox, oy);
            BSet(dst, x + ox, y + oy, c);
        }
        if (ox > dst->w)
            break;
    }
    return true;
}

bool PasteImageClip(Image *dst, Image *src, int x, int y, int rx, int ry, int rw, int rh) {
    for (int ox = 0; ox < rw; ++ox)
        for (int oy = 0; oy < rh; ++oy)
            BSet(dst, ox + x, oy + y, PGet(src, ox + rx, oy + ry));
    return true;
}

bool CopyImage(Image *a, Image *img) {
    if (!InitImage(img, a->w, a->h))
        return false;
    memcpy(img->buf, a->buf, a->w * a->h * sizeof(unsigned int) + 1);
    return !!img->buf;
}

void PassthruImage(Image *img, int (*fn)(int x, int y, int col)) {
    int x, y;
    for (x = 0; x < img->w; ++x)
        for (y = 0; y < img->h; ++y)
            img->buf[y * img->w + x] = fn(x, y, PGet(img, x, y));
}

bool ScaleImage(Image *a, int nw, int nh, Image *img) {
    if (!InitImage(img, nw, nh))
        return false;
    
    int x_ratio = (int)((a->w << 16) / img->w) + 1;
    int y_ratio = (int)((a->h << 16) / img->h) + 1;
    int x2, y2, i, j;
    for (i = 0; i < img->h; ++i) {
        int *t = img->buf + i * img->w;
        y2 = ((i * y_ratio) >> 16);
        int *p = a->buf + y2 * a->w;
        int rat = 0;
        for (j = 0; j < img->w; ++j) {
            x2 = (rat >> 16);
            *t++ = p[x2];
            rat += x_ratio;
        }
    }
    return true;
}

#define __MIN(a, b) (((a) < (b)) ? (a) : (b))
#define __MAX(a, b) (((a) > (b)) ? (a) : (b))
#define __D2R(a) ((a) * M_PI / 180.0)

bool RotateImage(Image *a, float angle, Image *img) {
    float theta = __D2R(angle);
    float c = cosf(theta), s = sinf(theta);
    float r[3][2] = {
        { -a->h * s, a->h * c },
        {  a->w * c - a->h * s, a->h * c + a->w * s },
        {  a->w * c, a->w * s }
    };
    
    float mm[2][2] = {{
        __MIN(0, __MIN(r[0][0], __MIN(r[1][0], r[2][0]))),
        __MIN(0, __MIN(r[0][1], __MIN(r[1][1], r[2][1])))
    }, {
        (theta > 1.5708  && theta < 3.14159 ? 0.f : __MAX(r[0][0], __MAX(r[1][0], r[2][0]))),
        (theta > 3.14159 && theta < 4.71239 ? 0.f : __MAX(r[0][1], __MAX(r[1][1], r[2][1])))
    }};
    
    int dw = (int)ceil(fabsf(mm[1][0]) - mm[0][0]);
    int dh = (int)ceil(fabsf(mm[1][1]) - mm[0][1]);
    if (!InitImage(img, dw, dh))
        return false;
    
    int x, y, sx, sy;
    for (x = 0; x < dw; ++x)
        for (y = 0; y < dh; ++y) {
            sx = ((x + mm[0][0]) * c + (y + mm[0][1]) * s);
            sy = ((y + mm[0][1]) * c - (x + mm[0][0]) * s);
            if (sx < 0 || sx >= a->w || sy < 0 || sy >= a->h)
                continue;
            BSet(img, x, y, PGet(a, sx, sy));
        }
    return true;
}

static inline void vline(Image *img, int x, int y0, int y1, int col) {
    if (y1 < y0) {
        y0 += y1;
        y1  = y0 - y1;
        y0 -= y1;
    }
    
    if (x < 0 || x >= img->w || y0 >= img->h)
        return;
    
    if (y0 < 0)
        y0 = 0;
    if (y1 >= img->h)
        y1 = img->h - 1;
    
    for(int y = y0; y <= y1; y++)
        BSet(img, x, y, col);
}

static inline void hline(Image *img, int y, int x0, int x1, int col) {
    if (x1 < x0) {
        x0 += x1;
        x1  = x0 - x1;
        x0 -= x1;
    }
    
    if (y < 0 || y >= img->h || x0 >= img->w)
        return;
    
    if (x0 < 0)
        x0 = 0;
    if (x1 >= img->w)
        x1 = img->w - 1;
    
    for(int x = x0; x <= x1; x++)
        BSet(img, x, y, col);
}

void DrawLine(Image *img, int x0, int y0, int x1, int y1, int col) {
    if (x0 == x1)
        vline(img, x0, y0, y1, col);
    if (y0 == y1)
        hline(img, y0, x0, x1, col);
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    
    while (BSet(img, x0, y0, col), x0 != x1 || y0 != y1) {
        int e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 <  dy) { err += dx; y0 += sy; }
    }
}

void DrawCircle(Image *img, int xc, int yc, int r, int col, bool fill) {
    int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
    do {
        BSet(img, xc - x, yc + y, col);    /*   I. Quadrant */
        BSet(img, xc - y, yc - x, col);    /*  II. Quadrant */
        BSet(img, xc + x, yc - y, col);    /* III. Quadrant */
        BSet(img, xc + y, yc + x, col);    /*  IV. Quadrant */
        
        if (fill) {
            hline(img, yc - y, xc - x, xc + x, col);
            hline(img, yc + y, xc - x, xc + x, col);
        }
        
        r = err;
        if (r <= y)
            err += ++y * 2 + 1; /* e_xy+e_y < 0 */
        if (r > x || err > y)
            err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);
}

void DrawRect(Image *img, int x, int y, int w, int h, int col, bool fill) {
    if (x < 0) {
        w += x;
        x  = 0;
    }
    if (y < 0) {
        h += y;
        y  = 0;
    }
    
    w += x;
    h += y;
    if (w < 0 || h < 0 || x > img->w || y > img->h)
        return;
    
    if (w > img->w)
        w = img->w;
    if (h > img->h)
        h = img->h;
    
    if (fill) {
        for (; y < h; ++y)
            hline(img, y, x, w, col);
    } else {
        hline(img, y, x, w, col);
        hline(img, h, x, w, col);
        vline(img, x, y, h, col);
        vline(img, w, y, h, col);
    }
}

#define __SWAP(a, b)  \
    do                \
    {                 \
        int temp = a; \
        a = b;        \
        b = temp;     \
    } while (0)

void DrawTri(Image *img, int x0, int y0, int x1, int y1, int x2, int y2, int col, bool fill) {
    if (y0 ==  y1 && y0 ==  y2)
        return;
    if (fill) {
        if (y0 > y1) {
            __SWAP(x0, x1);
            __SWAP(y0, y1);
        }
        if (y0 > y2) {
            __SWAP(x0, x2);
            __SWAP(y0, y2);
        }
        if (y1 > y2) {
            __SWAP(x1, x2);
            __SWAP(y1, y2);
        }
        
        int total_height = y2 - y0, i, j;
        for (i = 0; i < total_height; ++i) {
            bool second_half = i > y1 - y0 || y1 == y0;
            int segment_height = second_half ? y2 - y1 : y1 - y0;
            float alpha = (float)i / total_height;
            float beta  = (float)(i - (second_half ? y1 - y0 : 0)) / segment_height;
            int ax = x0 + (x2 - x0) * alpha;
            int ay = y0 + (y2 - y0) * alpha;
            int bx = second_half ? x1 + (x2 - x1) : x0 + (x1 - x0) * beta;
            int by = second_half ? y1 + (y2 - y1) : y0 + (y1 - y0) * beta;
            if (ax > bx) {
                __SWAP(ax, bx);
                __SWAP(ay, by);
            }
            for (j = ax; j <= bx; ++j)
                BSet(img, j, y0 + i, col);
        }
    } else {
        DrawLine(img, x0, y0, x1, y1, col);
        DrawLine(img, x1, y1, x2, y2, col);
        DrawLine(img, x2, y2, x0, y0, col);
    }
}

typedef struct {
    const unsigned char *p, *end;
} PNG;

static unsigned get32(const unsigned char* v) {
    return (v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
}

static const unsigned char* find(PNG* png, const char* chunk, unsigned minlen) {
    const unsigned char* start;
    while (png->p < png->end) {
        unsigned len = get32(png->p + 0);
        start = png->p;
        png->p += len + 12;
        if (memcmp(start + 4, chunk, 4) == 0 && len >= minlen && png->p <= png->end)
            return start + 8;
    }
    return NULL;
}

static unsigned char paeth(unsigned char a, unsigned char b, unsigned char c) {
    int p = a + b - c;
    int pa = abs(p - a), pb = abs(p - b), pc = abs(p - c);
    return (pa <= pb && pa <= pc) ? a : (pb <= pc) ? b : c;
}

static int rowBytes(int w, int bipp) {
    int rowBits = w * bipp;
    return rowBits / 8 + ((rowBits % 8) ? 1 : 0);
}

static int unfilter(int w, int h, int bipp, unsigned char* raw) {
    int len = rowBytes(w, bipp);
    int bpp = rowBytes(1, bipp);
    int x, y;
    unsigned char* first = (unsigned char*)malloc(len + 1);
    memset(first, 0, len + 1);
    unsigned char* prev = first;
    for (y = 0; y < h; y++, prev = raw, raw += len) {
#define LOOP(A, B)            \
    for (x = 0; x < bpp; x++) \
        raw[x] += A;          \
    for (; x < len; x++)      \
        raw[x] += B;          \
    break
        switch (*raw++) {
            case 0:
                break;
            case 1:
                LOOP(0, raw[x - bpp]);
            case 2:
                LOOP(prev[x], prev[x]);
            case 3:
                LOOP(prev[x] / 2, (raw[x - bpp] + prev[x]) / 2);
            case 4:
                LOOP(prev[x], paeth(raw[x - bpp], prev[x], prev[x - bpp]));
            default:
                return 0;
        }
#undef LOOP
    }
    free(first);
    return 1;
}

static void convert(int bypp, int w, int h, const unsigned char* src, int* dest, const unsigned char* trns) {
    int x, y;
    for (y = 0; y < h; y++) {
        src++;  // skip filter byte
        for (x = 0; x < w; x++, src += bypp) {
            switch (bypp) {
                case 1: {
                    unsigned char c = src[0];
                    if (trns && c == *trns) {
                        *dest++ = RGBA1(c, 0);
                        break;
                    } else {
                        *dest++ = RGB1(c);
                        break;
                    }
                }
                case 2:
                    *dest++ = RGBA(src[0], src[0], src[0], src[1]);
                    break;
                case 3: {
                    unsigned char r = src[0];
                    unsigned char g = src[1];
                    unsigned char b = src[2];
                    if (trns && trns[1] == r && trns[3] == g && trns[5] == b) {
                        *dest++ = RGBA(r, g, b, 0);
                        break;
                    } else {
                        *dest++ = RGB(r, g, b);
                        break;
                    }
                }
                case 4:
                    *dest++ = RGBA(src[0], src[1], src[2], src[3]);
                    break;
            }
        }
    }
}

static void depalette(int w, int h, unsigned char* src, int* dest, int bipp, const unsigned char* plte, const unsigned char* trns, int trnsSize) {
    int x, y, c;
    unsigned char alpha;
    int mask, len;

    switch (bipp) {
        case 4:
            mask = 15;
            len = 1;
            break;
        case 2:
            mask = 3;
            len = 3;
            break;
        case 1:
            mask = 1;
            len = 7;
    }

    for (y = 0; y < h; y++) {
        src++;  // skip filter byte
        for (x = 0; x < w; x++) {
            if (bipp == 8) {
                c = *src++;
            } else {
                int pos = x & len;
                c = (src[0] >> ((len - pos) * bipp)) & mask;
                if (pos == len) {
                    src++;
                }
            }
            alpha = 255;
            if (c < trnsSize) {
                alpha = trns[c];
            }
            *dest++ = RGBA(plte[c * 3 + 0], plte[c * 3 + 1], plte[c * 3 + 2], alpha);
        }
    }
}

static int outsize(Image* bmp, int bipp) {
    return (rowBytes(bmp->w, bipp) + 1) * bmp->h;
}

#define PNG_FAIL()      \
    {                   \
        errno = EINVAL; \
        goto err;       \
    }
#define PNG_CHECK(X) \
    if (!(X))        \
        PNG_FAIL()

typedef struct {
    unsigned bits, count;
    const unsigned char *in, *inend;
    unsigned char *out, *outend;
    jmp_buf jmp;
    unsigned litcodes[288], distcodes[32], lencodes[19];
    int tlit, tdist, tlen;
} State;

#define INFLATE_FAIL() longjmp(s->jmp, 1)
#define INFLATE_CHECK(X) \
    if (!(X))            \
        INFLATE_FAIL()

// Built-in DEFLATE standard tables.
static char order[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
static char lenBits[29 + 2] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
                                3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0 };
static int lenBase[29 + 2] = { 3,  4,  5,  6,  7,  8,  9,  10,  11,  13,  15,  17,  19,  23, 27, 31,
                               35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0,  0 };
static char distBits[30 + 2] = { 0, 0, 0, 0, 1, 1, 2,  2,  3,  3,  4,  4,  5,  5,  6, 6,
                                 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 0, 0 };
static int distBase[30 + 2] = {
    1,   2,   3,   4,   5,   7,    9,    13,   17,   25,   33,   49,   65,    97,    129,
    193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
};

static const unsigned char reverseTable[256] = {
#define R2(n) n, n + 128, n + 64, n + 192
#define R4(n) R2(n), R2(n + 32), R2(n + 16), R2(n + 48)
#define R6(n) R4(n), R4(n + 8), R4(n + 4), R4(n + 12)
    R6(0), R6(2), R6(1), R6(3)
};

static unsigned rev16(unsigned n) {
    return (reverseTable[n & 0xff] << 8) | reverseTable[(n >> 8) & 0xff];
}

static int bits(State* s, int n) {
    int v = s->bits & ((1 << n) - 1);
    s->bits >>= n;
    s->count -= n;
    while (s->count < 16) {
        INFLATE_CHECK(s->in != s->inend);
        s->bits |= (*s->in++) << s->count;
        s->count += 8;
    }
    return v;
}

static unsigned char* emit(State* s, int len) {
    s->out += len;
    INFLATE_CHECK(s->out <= s->outend);
    return s->out - len;
}

static void copy(State* s, const unsigned char* src, int len) {
    unsigned char* dest = emit(s, len);
    while (len--)
        *dest++ = *src++;
}

static int build(State* s, unsigned* tree, unsigned char* lens, int symcount) {
    int n, codes[16], first[16], counts[16] = { 0 };

    // Frequency count.
    for (n = 0; n < symcount; n++)
        counts[lens[n]]++;

    // Distribute codes.
    counts[0] = codes[0] = first[0] = 0;
    for (n = 1; n <= 15; n++) {
        codes[n] = (codes[n - 1] + counts[n - 1]) << 1;
        first[n] = first[n - 1] + counts[n - 1];
    }
    INFLATE_CHECK(first[15] + counts[15] <= symcount);

    // Insert keys into the tree for each symbol.
    for (n = 0; n < symcount; n++) {
        int len = lens[n];
        if (len != 0) {
            int code = codes[len]++, slot = first[len]++;
            tree[slot] = (code << (32 - len)) | (n << 4) | len;
        }
    }

    return first[15];
}

static int decode(State* s, unsigned tree[], int max) {
    // Find the next prefix code.
    unsigned lo = 0, hi = max, key;
    unsigned search = (rev16(s->bits) << 16) | 0xffff;
    while (lo < hi) {
        unsigned guess = (lo + hi) / 2;
        if (search < tree[guess])
            hi = guess;
        else
            lo = guess + 1;
    }

    // Pull out the key and check it.
    key = tree[lo - 1];
    INFLATE_CHECK(((search ^ key) >> (32 - (key & 0xf))) == 0);

    bits(s, key & 0xf);
    return (key >> 4) & 0xfff;
}

static void run(State* s, int sym) {
    int length = bits(s, lenBits[sym]) + lenBase[sym];
    int dsym = decode(s, s->distcodes, s->tdist);
    int offs = bits(s, distBits[dsym]) + distBase[dsym];
    copy(s, s->out - offs, length);
}

static void block(State* s) {
    for (;;) {
        int sym = decode(s, s->litcodes, s->tlit);
        if (sym < 256)
            *emit(s, 1) = (unsigned char)sym;
        else if (sym > 256)
            run(s, sym - 257);
        else
            break;
    }
}

static void stored(State* s) {
    // Uncompressed data block.
    int len;
    bits(s, s->count & 7);
    len = bits(s, 16);
    INFLATE_CHECK(((len ^ s->bits) & 0xffff) == 0xffff);
    INFLATE_CHECK(s->in + len <= s->inend);

    copy(s, s->in, len);
    s->in += len;
    bits(s, 16);
}

static void fixed(State* s) {
    // Fixed set of Huffman codes.
    int n;
    unsigned char lens[288 + 32];
    for (n = 0; n <= 143; n++)
        lens[n] = 8;
    for (n = 144; n <= 255; n++)
        lens[n] = 9;
    for (n = 256; n <= 279; n++)
        lens[n] = 7;
    for (n = 280; n <= 287; n++)
        lens[n] = 8;
    for (n = 0; n < 32; n++)
        lens[288 + n] = 5;

    // Build lit/dist trees.
    s->tlit = build(s, s->litcodes, lens, 288);
    s->tdist = build(s, s->distcodes, lens + 288, 32);
}

static void dynamic(State* s) {
    int n, i, nlit, ndist, nlen;
    unsigned char lenlens[19] = { 0 }, lens[288 + 32];
    nlit = 257 + bits(s, 5);
    ndist = 1 + bits(s, 5);
    nlen = 4 + bits(s, 4);
    for (n = 0; n < nlen; n++)
        lenlens[(unsigned char)order[n]] = (unsigned char)bits(s, 3);

    // Build the tree for decoding code lengths.
    s->tlen = build(s, s->lencodes, lenlens, 19);

    // Decode code lengths.
    for (n = 0; n < nlit + ndist;) {
        int sym = decode(s, s->lencodes, s->tlen);
        switch (sym) {
            case 16:
                for (i = 3 + bits(s, 2); i; i--, n++)
                    lens[n] = lens[n - 1];
                break;
            case 17:
                for (i = 3 + bits(s, 3); i; i--, n++)
                    lens[n] = 0;
                break;
            case 18:
                for (i = 11 + bits(s, 7); i; i--, n++)
                    lens[n] = 0;
                break;
            default:
                lens[n++] = (unsigned char)sym;
                break;
        }
    }

    // Build lit/dist trees.
    s->tlit = build(s, s->litcodes, lens, nlit);
    s->tdist = build(s, s->distcodes, lens + nlit, ndist);
}

static int inflate(void* out, unsigned outlen, const void* in, unsigned inlen) {
    int last;
    State* s = calloc(1, sizeof(State));

    // We assume we can buffer 2 extra bytes from off the end of 'in'.
    s->in = (unsigned char*)in;
    s->inend = s->in + inlen + 2;
    s->out = (unsigned char*)out;
    s->outend = s->out + outlen;
    s->bits = 0;
    s->count = 0;
    bits(s, 0);

    if (setjmp(s->jmp) == 1) {
        free(s);
        return 0;
    }

    do {
        last = bits(s, 1);
        switch (bits(s, 2)) {
            case 0:
                stored(s);
                break;
            case 1:
                fixed(s);
                block(s);
                break;
            case 2:
                dynamic(s);
                block(s);
                break;
            case 3:
                INFLATE_FAIL();
        }
    } while (!last);

    free(s);
    return 1;
}

static bool load_png(Image *img, PNG *png) {
    const unsigned char *ihdr, *idat, *plte, *trns, *first;
    int trnsSize = 0;
    int depth, ctype, bipp;
    int datalen = 0;
    unsigned char *data = NULL, *out;
    
    PNG_CHECK(memcmp(png->p, "\211PNG\r\n\032\n", 8) == 0);  // PNG signature
    png->p += 8;
    first = png->p;
    
    // Read IHDR
    ihdr = find(png, "IHDR", 13);
    PNG_CHECK(ihdr);
    depth = ihdr[8];
    ctype = ihdr[9];
    switch (ctype) {
        case 0:
            bipp = depth;
            break;  // greyscale
        case 2:
            bipp = 3 * depth;
            break;  // RGB
        case 3:
            bipp = depth;
            break;  // paletted
        case 4:
            bipp = 2 * depth;
            break;  // grey+alpha
        case 6:
            bipp = 4 * depth;
            break;  // RGBA
        default:
            PNG_FAIL();
    }
    
    // Allocate bitmap (+1 width to save room for stupid PNG filter bytes)
    InitImage(img, get32(ihdr + 0) + 1, get32(ihdr + 4));
    PNG_CHECK(img->buf);
    img->w--;
    
    // We support 8-bit color components and 1, 2, 4 and 8 bit palette formats.
    // No interlacing, or wacky filter types.
    PNG_CHECK((depth != 16) && ihdr[10] == 0 && ihdr[11] == 0 && ihdr[12] == 0);
    
    // Join IDAT chunks.
    for (idat = find(png, "IDAT", 0); idat; idat = find(png, "IDAT", 0)) {
        unsigned len = get32(idat - 8);
        data = realloc(data, datalen + len);
        if (!data)
            break;
        
        memcpy(data + datalen, idat, len);
        datalen += len;
    }
    
    // Find palette.
    png->p = first;
    plte = find(png, "PLTE", 0);
    
    // Find transparency info.
    png->p = first;
    trns = find(png, "tRNS", 0);
    if (trns) {
        trnsSize = get32(trns - 8);
    }
    
    PNG_CHECK(data && datalen >= 6);
    PNG_CHECK((data[0] & 0x0f) == 0x08  // compression method (RFC 1950)
          && (data[0] & 0xf0) <= 0x70   // window size
          && (data[1] & 0x20) == 0);    // preset dictionary present
    
    out = (unsigned char*)img->buf + outsize(img, 32) - outsize(img, bipp);
    PNG_CHECK(inflate(out, outsize(img, bipp), data + 2, datalen - 6));
    PNG_CHECK(unfilter(img->w, img->h, bipp, out));
    
    if (ctype == 3) {
        PNG_CHECK(plte);
        depalette(img->w, img->h, out, img->buf, bipp, plte, trns, trnsSize);
    } else {
        PNG_CHECK(bipp % 8 == 0);
        convert(bipp / 8, img->w, img->h, out, img->buf, trns);
    }
    
    free(data);
    return true;
    
err:
    if (data)
        free(data);
    if (img && img->buf)
        DestroyImage(img);
    return false;
}

static unsigned char* read_file(const char *path, size_t *sizeOfFile) {
    FILE *fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    unsigned char *data = malloc(size + 1);
    fread(data, size, 1, fp);
    fclose(fp);
    
    data[size] = '\0';
    *sizeOfFile = size;
    return data;
}

bool LoadImage(Image *out, const char *path) {
    unsigned char *data = NULL;
    size_t sizeOfData = 0;
    if (!(data = read_file(path, &sizeOfData)) && sizeOfData > 0)
        return false;
    bool result = LoadImageMemory(out, (void*)data, sizeOfData);
    free(data);
    return result;
}

bool LoadImageMemory(Image *out, const void *data, size_t sizeOfData) {
    PNG png = {
        .p = (unsigned char*)data,
        .end = (unsigned char*)data + sizeOfData
    };
    return load_png(out, &png);
}

typedef struct {
    unsigned crc, adler, bits, prev, runlen;
    FILE* out;
    unsigned crcTable[256];
} Save;

static const unsigned crctable[16] = { 0,          0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4,
                                       0x4db26158, 0x5005713c, 0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
                                       0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };

static void put(Save* s, unsigned v) {
    fputc(v, s->out);
    s->crc = (s->crc >> 4) ^ crctable[(s->crc & 15) ^ (v & 15)];
    s->crc = (s->crc >> 4) ^ crctable[(s->crc & 15) ^ (v >> 4)];
}

static void updateAdler(Save* s, unsigned v) {
    unsigned s1 = s->adler & 0xffff, s2 = (s->adler >> 16) & 0xffff;
    s1 = (s1 + v) % 65521;
    s2 = (s2 + s1) % 65521;
    s->adler = (s2 << 16) + s1;
}

static void put32(Save* s, unsigned v) {
    put(s, (v >> 24) & 0xff);
    put(s, (v >> 16) & 0xff);
    put(s, (v >> 8) & 0xff);
    put(s, v & 0xff);
}

void putbits(Save* s, unsigned data, unsigned bitcount) {
    while (bitcount--) {
        unsigned prev = s->bits;
        s->bits = (s->bits >> 1) | ((data & 1) << 7);
        data >>= 1;
        if (prev & 1) {
            put(s, s->bits);
            s->bits = 0x80;
        }
    }
}

void putbitsr(Save* s, unsigned data, unsigned bitcount) {
    while (bitcount--)
        putbits(s, data >> bitcount, 1);
}

static void begin(Save* s, const char* id, unsigned len) {
    put32(s, len);
    s->crc = 0xffffffff;
    put(s, id[0]);
    put(s, id[1]);
    put(s, id[2]);
    put(s, id[3]);
}

static void literal(Save* s, unsigned v) {
    // Encode a literal/length using the built-in tables.
    // Could do better with a custom table but whatever.
    if (v < 144)
        putbitsr(s, 0x030 + v - 0, 8);
    else if (v < 256)
        putbitsr(s, 0x190 + v - 144, 9);
    else if (v < 280)
        putbitsr(s, 0x000 + v - 256, 7);
    else
        putbitsr(s, 0x0c0 + v - 280, 8);
}

static void encodelen(Save* s, unsigned code, unsigned bits, unsigned len) {
    literal(s, code + (len >> bits));
    putbits(s, len, bits);
    putbits(s, 0, 5);
}

static void endrun(Save* s) {
    s->runlen--;
    literal(s, s->prev);

    if (s->runlen >= 67)
        encodelen(s, 277, 4, s->runlen - 67);
    else if (s->runlen >= 35)
        encodelen(s, 273, 3, s->runlen - 35);
    else if (s->runlen >= 19)
        encodelen(s, 269, 2, s->runlen - 19);
    else if (s->runlen >= 11)
        encodelen(s, 265, 1, s->runlen - 11);
    else if (s->runlen >= 3)
        encodelen(s, 257, 0, s->runlen - 3);
    else
        while (s->runlen--)
            literal(s, s->prev);
}

static void encodeByte(Save* s, unsigned char v) {
    updateAdler(s, v);

    // Simple RLE compression. We could do better by doing a search
    // to find matches, but this works pretty well TBH.
    if (s->prev == v && s->runlen < 115) {
        s->runlen++;
    } else {
        if (s->runlen)
            endrun(s);

        s->prev = v;
        s->runlen = 1;
    }
}

static void savePngHeader(Save* s, Image* bmp) {
    fwrite("\211PNG\r\n\032\n", 8, 1, s->out);
    begin(s, "IHDR", 13);
    put32(s, bmp->w);
    put32(s, bmp->h);
    put(s, 8);  // bit depth
    put(s, 6);  // RGBA
    put(s, 0);  // compression (deflate)
    put(s, 0);  // filter (standard)
    put(s, 0);  // interlace off
    put32(s, ~s->crc);
}

static long savePngData(Save* s, Image* bmp, long dataPos) {
    int x, y;
    long dataSize;
    begin(s, "IDAT", 0);
    put(s, 0x08);      // zlib compression method
    put(s, 0x1d);      // zlib compression flags
    putbits(s, 3, 3);  // zlib last block + fixed dictionary
    for (y = 0; y < bmp->h; y++) {
        int *row = &bmp->buf[y * bmp->w];
        int prev = RGBA1(0, 0);

        encodeByte(s, 1);  // sub filter
        for (x = 0; x < bmp->w; x++) {
            encodeByte(s, Rgba(row[x]) - Rgba(prev));
            encodeByte(s, rGba(row[x]) - rGba(prev));
            encodeByte(s, rgBa(row[x]) - rgBa(prev));
            encodeByte(s, rgbA(row[x]) - rgbA(prev));
            prev = row[x];
        }
    }
    endrun(s);
    literal(s, 256);  // terminator
    while (s->bits != 0x80)
        putbits(s, 0, 1);
    put32(s, s->adler);
    dataSize = (ftell(s->out) - dataPos) - 8;
    put32(s, ~s->crc);
    return dataSize;
}

bool SaveImage(Image *img, const char *path) {
    FILE* out = fopen(path, "wb");
    if (!out)
        return false;
    
    Save s;
    s.out = out;
    s.adler = 1;
    s.bits = 0x80;
    s.prev = 0xffff;
    s.runlen = 0;
    
    savePngHeader(&s, img);
    long dataPos = ftell(s.out);
    long dataSize = savePngData(&s, img, dataPos);
    
    // End chunk.
    begin(&s, "IEND", 0);
    put32(&s, ~s.crc);
    
    // Write back payload size.
    fseek(out, dataPos, SEEK_SET);
    put32(&s, dataSize);
    
    long err = ferror(out);
    fclose(out);
    return !err;
}

// MARK: Filesystem functions

bool DoesFileExist(const char *path) {
    return !access(path, F_OK);
}

bool DoesDirExist(const char *path) {
    struct stat sb;
    return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

char* FormatString(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int size = vsnprintf(NULL, 0, fmt, args) + 1;
    char *buf = malloc(size);
    vsnprintf(buf, size, fmt, args);
    va_end(args);
    return buf;
}

char* LoadFile(const char *path, size_t *length) {
    char *result = NULL;
    size_t sz = -1;
    FILE *fh = fopen(path, "rb");
    if (!fh)
        goto BAIL;
    fseek(fh, 0, SEEK_END);
    sz = ftell(fh);
    fseek(fh, 0, SEEK_SET);

    result = malloc(sz * sizeof(char));
    fread(result, sz, 1, fh);
    fclose(fh);
    
BAIL:
    if (length)
        *length = sz;
    return result;
}

// MARK: Random functions

void InitRandom(Random *r, unsigned int s) {
    r->seed = s;
    for (int i = 0; i < 17; i++) {
        s = s * 0xAC564B05 + 1;
        r->buffer[i] = s;
    }
    r->p1 = 0;
    r->p2 = 10;
}

#define ROTL(N, R) (((N) << (R)) | ((N) >> (32 - (R))))

unsigned int RandomBits(Random *r) {
    unsigned int result = r->buffer[r->p1] = ROTL(r->buffer[r->p2], 13) + ROTL(r->buffer[r->p1], 9);

    if (--r->p1 < 0)
        r->p1 = 16;
    if (--r->p2 < 0)
        r->p2 = 16;

    return result;
}

float RandomFloat(Random *r) {
    union {
        float value;
        unsigned int word;
    } convert = {
        .word = (RandomBits(r) >> 9) | 0x3f800000};
    return convert.value - 1.0f;
}

double RandomDouble(Random *r) {
    unsigned int bits = RandomBits(r);
    union {
        double value;
        unsigned int words[2];
    } convert = {
        .words = {
            bits << 20,
            (bits >> 12) | 0x3FF00000}};
    return convert.value - 1.0;
}

int RandomInt(Random *r, int max) {
    return RandomBits(r) % max;
}

float RandomFloatRange(Random *r, float min, float max) {
    return RandomFloat(r) * (max - min) + min;
}

double RandomDoubleRange(Random *r, double min, double max) {
    return RandomDouble(r) * (max - min) + min;
}

int RandomIntRange(Random *r, int min, int max) {
    return RandomBits(r) % (max + 1 - min) + min;
}

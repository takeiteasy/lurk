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

static char* LoadFile(const char *path, size_t *length) {
    char *result = NULL;
    size_t sz = -1;
    FILE *fh = fopen(path, "rb");
    if (!fh)
        goto BAIL;
    fseek(fh, 0, SEEK_END);
    sz = ftell(fh);
    fseek(fh, 0, SEEK_SET);

    result = (char*)malloc(sz * sizeof(char));
    fread(result, sz, 1, fh);
    fclose(fh);

BAIL:
    if (length)
        *length = sz;
    return result;
}

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

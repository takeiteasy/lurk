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
#include "framebuffer.glsl.h"
#include "sokol_time.h"
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_IMPLEMENTATION
#include "nuklear.h"
#include "sokol_nuklear.h"
#define MJSON_IMPL
#include "mjson.h"
#define JIM_IMPLEMENTATION
#include "jim.h"

#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#if defined(WEE_WINDOWS)
#include <io.h>
#define F_OK    0
#define access _access
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
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

// MARK: state

static struct {
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
} state = {
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

// MARK: Config/Argument parsing function

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

static void Usage(const char *name) {
    printf("  usage: ./%s [options]\n\n  options:\n", name);
    printf("\t  help (flag) -- Show this message\n");
    printf("\t  config (string) -- Path to .json config file (default: %s)\n", ConfigPath());
#define X(NAME, TYPE, VAL, DEFAULT, DOCS) \
    printf("\t  %s (%s) -- %s (default: %d)\n", NAME, #TYPE, DOCS, DEFAULT);
    SETTINGS
#undef X
}

static int LoadConfig(const char *path) {
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

static int ParseArguments(int argc, const char *argv[]) {
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
        if (!FileExists(path)) {
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

void UpdateTimer(Query *query) {
    Timer *timer = ECS_FIELD(query, Timer, 0);
    if (!timer->enabled)
        return;
    if (stm_ms(stm_since(timer->start)) > timer->interval) {
//        timer->cb(timer->userdata);
        timer->start = stm_now();
    }
}

// MARK: Event+Window functions

bool IsKeyDown(sapp_keycode key) {
    return state.input.button_down[key];
}

bool IsKeyUp(sapp_keycode key) {
    return !state.input.button_down[key];
}

bool WasKeyClicked(sapp_keycode key) {
    return state.input.button_clicked[key];
}

bool IsButtonDown(sapp_mousebutton button) {
    return state.input.mouse_down[button];
}

bool IsButtonUp(sapp_mousebutton button) {
    return !state.input.mouse_down[button];
}

bool WasButtonPressed(sapp_mousebutton button) {
    return state.input.mouse_clicked[button];
}

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

void InitCallback(void) {
    if (state.mouseHidden)
        sapp_show_mouse(false);
    if (state.mouseLocked)
        sapp_lock_mouse(true);
    
    sg_desc desc = (sg_desc) {
        // TODO: Add more configuration options for sg_desc
        .context = sapp_sgcontext()
    };
    sg_setup(&desc);
    
    snk_desc_t nk_desc = (snk_desc_t){0};
    snk_setup(&nk_desc);
    
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
    
    world.nextAvailableId = EcsNil;
    EcsSystem   = ECS_COMPONENT(System);
    EcsPrefab   = ECS_COMPONENT(Prefab);
    EcsRelation = ECS_COMPONENT(Relation);
    EcsChildOf  = ECS_TAG(result);
    EcsTimer    = ECS_COMPONENT(Timer);
    ECS_SYSTEM(UpdateTimer, EcsTimer);
}

void FrameCallback(void) {
    const int width = sapp_width();
    const int height = sapp_height();
    
    sg_begin_pass(state.pass, &state.pass_action);
    sg_end_pass();
    
    sg_begin_default_pass(&state.pass_action, width, height);
    sg_apply_pipeline(state.pip);

    sg_apply_bindings(&state.bind);
    sg_draw(0, 6, 1);
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

void EventCallback(const sapp_event* e) {
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

void CleanupCallback(void) {
    state.running = false;
    sg_destroy_pass(state.pass);
    sg_destroy_pipeline(state.pip);
    sg_destroy_image(state.color);
    sg_destroy_image(state.depth);
    sg_shutdown();
}

int main(int argc, const char *argv[]) {
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
    if (argc > 1)
        if (!ParseArguments(argc, argv)) {
            fprintf(stderr, "[PARSE ARGUMENTS ERROR] Failed to parse arguments\n");
            return EXIT_FAILURE;
        }
    
    state.desc.init_cb = InitCallback;
    state.desc.frame_cb = FrameCallback;
    state.desc.event_cb = EventCallback;
    state.desc.cleanup_cb = CleanupCallback;
    sapp_run(&state.desc);
    return 0;
}

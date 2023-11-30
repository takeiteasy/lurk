/* gamekit.c -- https://github.com/takeiteasy/c-gamekit

 The MIT License (MIT)
 Copyright (c) 2022 George Watson
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge,
 publish, distribute, sublicense, and/or sell copies of the Software,
 and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#define SOKOL_IMPL
#define JIM_IMPLEMENTATION
#define MJSON_IMPLEMENTATION
#define HASHMAP_IMPL
#define QOI_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define EZ_IMPLEMENTATION
#define IMAP_IMPLEMENTATION
#include "gamekit.h"

#if !defined(GAMEKIT_STATE)
static gkTexture* NewTexture(sg_image_desc *desc) {
    gkTexture *result = malloc(sizeof(gkTexture));
    result->internal = sg_make_image(desc);
    result->w = desc->width;
    result->h = desc->height;
    return result;
}

static gkTexture* EmptyTexture(unsigned int w, unsigned int h) {
    sg_image_desc desc = {
        .width = w,
        .height = h,
//        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .usage = SG_USAGE_STREAM
    };
    return NewTexture(&desc);
}

static void DestroyTexture(gkTexture *texture) {
    if (texture) {
        if (sg_query_image_state(texture->internal) == SG_RESOURCESTATE_VALID)
            sg_destroy_image(texture->internal);
        free(texture);
    }
}

#define QOI_MAGIC (((unsigned int)'q') << 24 | ((unsigned int)'o') << 16 | ((unsigned int)'i') <<  8 | ((unsigned int)'f'))

static bool CheckQOI(unsigned char *data) {
    return (data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3]) == QOI_MAGIC;
}

#define RGBA(R, G, B, A) (((unsigned int)(A) << 24) | ((unsigned int)(R) << 16) | ((unsigned int)(G) << 8) | (B))

static int* LoadImage(unsigned char *data, int sizeOfData, int *w, int *h) {
    assert(data && sizeOfData);
    int _w, _h, c;
    unsigned char *in = NULL;
    if (CheckQOI(data)) {
        qoi_desc desc;
        in = qoi_decode(data, sizeOfData, &desc, 4);
        _w = desc.width;
        _h = desc.height;
    } else
        in = stbi_load_from_memory(data, sizeOfData, &_w, &_h, &c, 4);
    assert(in && _w && _h);
    
    int *buf = malloc(_w * _h * sizeof(int));
    for (int x = 0; x < _w; x++)
        for (int y = 0; y < _h; y++) {
            unsigned char *p = in + (x + _w * y) * 4;
            buf[y * _w + x] = RGBA(p[0], p[1], p[2], p[3]);
        }
    free(in);
    if (w)
        *w = _w;
    if (h)
        *h = _h;
    return buf;
}

static void UpdateTexture(gkTexture *texture, int *data, int w, int h) {
    if (texture->w != w || texture->h != h) {
        DestroyTexture(texture);
        texture = EmptyTexture(w, h);
    }
    sg_image_data desc = {
        .subimage[0][0] = (sg_range) {
            .ptr = data,
            .size = w * h * sizeof(int)
        }
    };
    sg_update_image(texture->internal, &desc);
}

gkState state = {
    .running = false,
    .desc = (sapp_desc) {
#define X(NAME, TYPE, VAL, DEFAULT, DOCS) .VAL = DEFAULT,
        SETTINGS
#undef X
        .window_title = DEFAULT_WINDOW_TITLE
    },
    .pass_action = {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = {0}
        }
    }
};

static gkState *currentState = NULL;
#endif

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
//-----------------------------------------------------------------------------
static void MM86128(const void *key, const int len, uint32_t seed, void *out) {
#define ROTL32(x, r) ((x << r) | (x >> (32 - r)))
#define FMIX32(h) h^=h>>16; h*=0x85ebca6b; h^=h>>13; h*=0xc2b2ae35; h^=h>>16;
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 16;
    uint32_t h1 = seed;
    uint32_t h2 = seed;
    uint32_t h3 = seed;
    uint32_t h4 = seed;
    uint32_t c1 = 0x239b961b;
    uint32_t c2 = 0xab0e9789;
    uint32_t c3 = 0x38b34ae5;
    uint32_t c4 = 0xa1e38b93;
    const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);
    for (int i = -nblocks; i; i++) {
        uint32_t k1 = blocks[i*4+0];
        uint32_t k2 = blocks[i*4+1];
        uint32_t k3 = blocks[i*4+2];
        uint32_t k4 = blocks[i*4+3];
        k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
        h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;
        k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
        h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;
        k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
        h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;
        k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
        h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
    }
    const uint8_t * tail = (const uint8_t*)(data + nblocks*16);
    uint32_t k1 = 0;
    uint32_t k2 = 0;
    uint32_t k3 = 0;
    uint32_t k4 = 0;
    switch(len & 15) {
        case 15:
            k4 ^= tail[14] << 16;
        case 14:
            k4 ^= tail[13] << 8;
        case 13:
            k4 ^= tail[12] << 0;
            k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
        case 12:
            k3 ^= tail[11] << 24;
        case 11:
            k3 ^= tail[10] << 16;
        case 10:
            k3 ^= tail[ 9] << 8;
        case 9:
            k3 ^= tail[ 8] << 0;
            k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
        case 8:
            k2 ^= tail[ 7] << 24;
        case 7:
            k2 ^= tail[ 6] << 16;
        case 6:
            k2 ^= tail[ 5] << 8;
        case 5:
            k2 ^= tail[ 4] << 0;
            k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
        case 4:
            k1 ^= tail[ 3] << 24;
        case 3:
            k1 ^= tail[ 2] << 16;
        case 2:
            k1 ^= tail[ 1] << 8;
        case 1:
            k1 ^= tail[ 0] << 0;
            k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };
    h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;
    h1 += h2; h1 += h3; h1 += h4;
    h2 += h1; h3 += h1; h4 += h1;
    FMIX32(h1); FMIX32(h2); FMIX32(h3); FMIX32(h4);
    h1 += h2; h1 += h3; h1 += h4;
    h2 += h1; h3 += h1; h4 += h1;
    ((uint32_t*)out)[0] = h1;
    ((uint32_t*)out)[1] = h2;
    ((uint32_t*)out)[2] = h3;
    ((uint32_t*)out)[3] = h4;
}

static uint64_t MurmurHash(const void *data, size_t len, uint32_t seed) {
    char out[16];
    MM86128(data, (int)len, (uint32_t)seed, &out);
    return *(uint64_t*)out;
}

typedef struct {
    const char *name;
    gkInternalScene *wis;
} SceneBucket;

#if defined(GAMEKIT_WINDOWS)
static FILETIME Win32GetLastWriteTime(char* path) {
    FILETIME time;
    WIN32_FILE_ATTRIBUTE_DATA data;

    if (GetFileAttributesEx(path, GetFileExInfoStandard, &data))
        time = data.ftLastWriteTime;

    return time;
}
#endif

#if !defined(GAMEKIT_STATE)
static bool ShouldReloadLibrary(gkInternalScene *wis) {
#if defined(GAMEKIT_WINDOWS)
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

static bool ReloadLibrary(gkInternalScene *wis) {
#if !defined(GAMEKIT_DISABLE_SCENE_RELOAD)
    assert(wis);
    if (!ShouldReloadLibrary(wis))
        return true;
#endif
    
    if (wis->handle) {
        if (wis->scene->unload)
            wis->scene->unload(currentState, wis->context);
        dlclose(wis->handle);
    }
    
#if defined(GAMEKIT_WINDOWS)
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
#if defined(GAMEKIT_WINDOWS)
    memset(&writeTime, 0, sizeof(FILETIME));
#else
    wis->handleID = 0;
#endif
    return false;
}

static void Usage(const char *name) {
    printf("  usage: %s [options]\n\n  options:\n", name);
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
#if defined GAMEKIT_EMSCRIPTEN
        .argc = argc,
        .argv = (char**)argv
#else
        .argc = argc - 1,
        .argv = (char**)(argv + 1)
#endif
    };
    sargs_setup(&desc);
    
#if !defined(GAMEKIT_EMSCRIPTEN)
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
#endif // GAMEKIT_EMSCRIPTEN
    
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

#define VALID_EXTS_LEN 9
static const char *validImages[VALID_EXTS_LEN] = {
    "jpg",
    "png",
    "tga",
    "bmp",
    "psd",
    "gdr",
    "pic",
    "pnm",
    "qoi"
};

static const char* ToLower(const char *str, int length) {
    if (!length)
        length = (int)strlen(str);
    assert(length);
    char *result = malloc(sizeof(char) * length);
    for (int i = 0; i < length; i++) {
        char c = str[i];
        result[i] = isalpha(c) && isupper(c) ? tolower(c) : c;
    }
    return result;
}

static void InitCallback(void) {
    sg_desc desc = (sg_desc) {
        // TODO: Add more configuration options for sg_desc
        .context = sapp_sgcontext()
    };
    sg_setup(&desc);
    stm_setup();
    sgp_desc desc_sgp = (sgp_desc) {};
    sgp_setup(&desc_sgp);
    assert(sg_isvalid() && sgp_is_valid());
    
    state.textureMapCapacity = 1;
    state.textureMapCount = 0;
    state.textureMap = imap_ensure(NULL, 1);
    state.assets = ezContainerRead(GAMEKIT_ASSETS_PATH);
    for (int i = 0; i < state.assets->sizeOfEntries; i++) {
        ezContainerTreeEntry *e = &state.assets->entries[i];
        const char *ext = FileExt(e->filePath);
        const char *extLower = ToLower(ext, 0);
        
        for (int i = 0; i < VALID_EXTS_LEN; i++)
            if (!strncmp(validImages[i], extLower, 3)) {
                if (++state.textureMapCount > state.textureMapCapacity) {
                    state.textureMapCapacity += state.textureMapCapacity;
                    state.textureMap = imap_ensure(state.textureMap, state.textureMapCapacity);
                }
                const char *fname = FileName(e->filePath);
                uint64_t hash = MurmurHash((void*)fname, strlen(fname), 0);
                imap_slot_t *slot = imap_assign(state.textureMap, hash);
                unsigned char *data = ezContainerEntryRaw(state.assets, &e->entry);
                int w, h;
                int *buf = LoadImage(data, (int)e->entry.fileSize, &w, &h);
                free(data);
                gkTexture *texture = EmptyTexture(w, h);
                UpdateTexture(texture, buf, w, h);
                free(buf);
                imap_setval64(state.textureMap, slot, (uint64_t)texture);
            }
        // TODO: Check other asset types
        free((void*)extLower);
    }
    
    state.windowWidth = sapp_width();
    state.windowHeight = sapp_height();
    state.clearColor = (sg_color){0.39f, 0.58f, 0.92f, 1.f};
    
#if defined(GAMEKIT_MAC)
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    uint64_t frequency = info.denom;
    frequency *= 1000000000L;
    state.timerFrequency = frequency / info.numer;
#elif defined(GAMEKIT_WINDOW)
    LARGE_INTEGER frequency;
    if (!QueryPerformanceFrequency(&frequency))
        return 1000L;
    state.timerFrequency = frequency.QuadPart;
#else
    state.timerFrequency = 1000000000L;
#endif
    
    state.updateMultiplicity = 1;
#if defined(GAMEKIT_UNLOCKFRAME_RATE)
    state.unlockFramerate = 1;
#else
    state.unlockFramerate = 0;
#endif
    state.desiredFrameTime = state.timerFrequency * DEFAULT_TARGET_FPS;
    state.fixedDeltaTime = 1.0 / DEFAULT_TARGET_FPS;
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
    
    currentState = &state;
#if defined(GAMEKIT_MAC)
#define DYLIB_EXT ".dylib"
#elif defined(GAMEKIT_WINDOWS)
#define DYLIB_EXT ".dll"
#elif defined(GAMEKIT_LINUX)
#define DYLIB_EXT ".so"
#else
#error Unsupported operating system
#endif
    
#if !defined(GAMEKIT_DYLIB_PATH)
#define GAMEKIT_DYLIB_PATH ResolvePath("./");
#endif
    
    int sceneCount = 0;
#define X(NAME) sceneCount++;
GAMEKIT_SCENES
#undef X
    state.sceneMap = imap_ensure(NULL, sceneCount);
#define X(NAME)                                                       \
    do {                                                              \
        uint64_t hash = MurmurHash((void*)(NAME), strlen((NAME)), 0); \
        imap_slot_t *slot = imap_assign(state.sceneMap, hash);        \
        gkInternalScene *wis = malloc(sizeof(gkInternalScene));     \
        wis->path = GAMEKIT_DYLIB_PATH NAME DYLIB_EXT;                    \
        wis->context = NULL;                                          \
        wis->scene = NULL;                                            \
        wis->handle = NULL;                                           \
        assert(ReloadLibrary(wis));                                   \
        imap_setval64(state.sceneMap, slot, (uint64_t)wis);           \
    } while (0);
GAMEKIT_SCENES
#undef X
    gkPushScene(&state, GAMEKIT_FIRST_SCENE);
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
    
#if !defined(GAMEKIT_DISABLE_SCENE_RELOAD)
    if (state.currentScene)
        assert(ReloadLibrary(state.currentScene));
#endif
    
    if (state.currentScene && state.currentScene->scene->preframe)
        state.currentScene->scene->preframe(&state, state.currentScene->context);
    
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
            if (state.currentScene && state.currentScene->scene->fixedupdate)
                state.currentScene->scene->fixedupdate(&state, state.currentScene->context, state.fixedDeltaTime);
            if (consumedDeltaTime > state.desiredFrameTime) {
                if (state.currentScene && state.currentScene->scene->update)
                    state.currentScene->scene->update(&state, state.currentScene->context, state.fixedDeltaTime);
                consumedDeltaTime -= state.desiredFrameTime;
            }
            state.frameAccumulator -= state.desiredFrameTime;
        }
        
        if (state.currentScene && state.currentScene->scene->update)
            state.currentScene->scene->update(&state, state.currentScene->context, (double)consumedDeltaTime / state.timerFrequency);
        render_time = (double)state.frameAccumulator / state.desiredFrameTime;
    } else
        while (state.frameAccumulator >= state.desiredFrameTime*state.updateMultiplicity)
            for (int i = 0; i < state.updateMultiplicity; ++i) {
                if (state.currentScene && state.currentScene->scene->fixedupdate)
                    state.currentScene->scene->fixedupdate(&state, state.currentScene->context, state.fixedDeltaTime);
                if (state.currentScene && state.currentScene->scene->update)
                    state.currentScene->scene->update(&state, state.currentScene->context, state.fixedDeltaTime);
                state.frameAccumulator -= state.desiredFrameTime;
            }
    
    sgp_begin(state.windowWidth, state.windowHeight);
    sgp_set_color(state.clearColor.r,
                  state.clearColor.g,
                  state.clearColor.b,
                  state.clearColor.a);
    sgp_clear();
    if (state.currentScene && state.currentScene->scene->frame)
        state.currentScene->scene->frame(&state, state.currentScene->context, render_time);
    // Assemble draw calls here
    
    sg_begin_default_pass(&state.pass_action, state.windowWidth, state.windowHeight);
    sgp_flush();
    sgp_end();
    sg_end_pass();
    sg_commit();
    
    if (state.currentScene && state.currentScene->scene->postframe)
        state.currentScene->scene->postframe(&state, state.currentScene->context);
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
    if (state.currentScene && state.currentScene->scene->event)
        state.currentScene->scene->event(&state, state.currentScene->context, e);
}

static void CleanupCallback(void) {
    state.running = false;
    ezContainerFree(state.assets);
#define X(NAME)                                                                         \
    do {                                                                                \
        uint64_t hash = MurmurHash((void*)(NAME), strlen((NAME)), 0);                   \
        imap_slot_t *slot = imap_lookup(state.sceneMap, hash);                          \
        assert(slot);                                                                   \
        gkInternalScene *wis = (gkInternalScene*)imap_getval64(state.sceneMap, slot); \
        free(wis);                                                                      \
    } while (0);
GAMEKIT_SCENES
#undef X
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
#if defined(GAMEKIT_ENABLE_CONFIG)
#if !defined(GAMEKIT_CONFIG_PATH)
    const char *configPath = JoinPath(UserPath(), DEFAULT_CONFIG_NAME);
#else
    const char *configPath = ResolvePath(GAMEKIT_CONFIG_PATH);
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
#if defined(GAMEKIT_ENABLE_ARGUMENTS)
    if (argc > 1)
        if (!ParseArguments(argc, argv)) {
            fprintf(stderr, "[PARSE ARGUMENTS ERROR] Failed to parse arguments\n");
            abort();
        }
#endif
    
    state.desc.init_cb = InitCallback;
    state.desc.frame_cb = FrameCallback;
    state.desc.event_cb = EventCallback;
    state.desc.cleanup_cb = CleanupCallback;
    return state.desc;
}
#endif

void gkPushScene(gkState *state, const char *name) {
    uint64_t hash = MurmurHash((void*)name, strlen(name), 0);
    imap_slot_t *slot = imap_lookup(state->sceneMap, hash);
    assert(slot);
    gkInternalScene *wis = (gkInternalScene*)imap_getval64(state->sceneMap, slot);
    bool reload = false;
    if (state->currentScene) {
        wis->next = state->currentScene;
        if (state->currentScene->scene->unload)
            state->currentScene->scene->unload(state, state->currentScene->context);
        reload = true;
    }
    state->currentScene = wis;
    if (reload && state->currentScene->scene->reload)
        state->currentScene->scene->reload(state, state->currentScene->context);
}

void gkPopScene(gkState *state) {
    if (state)
        sapp_quit();
    else {
        if (state->currentScene->scene->unload)
            state->currentScene->scene->unload(state, state->currentScene->context);
        state->currentScene = state->currentScene->next;
    }
}

int gkWindowWidth(gkState *state) {
    return state->windowWidth;
}

int gkWindowHeight(gkState *state) {
    return state->windowHeight;
}

int gkIsWindowFullscreen(gkState *state) {
    return state->fullscreen;
}

void gkToggleFullscreen(gkState *state) {
    state->fullscreen = !state->fullscreen;
}

int gkIsCursorVisible(gkState *state) {
    return state->cursorVisible;
}

void gkToggleCursorVisible(gkState *state) {
    state->cursorVisible = !state->cursorVisible;
}

int gkIsCursorLocked(gkState *state) {
    return state->cursorLocked;
}

void gkToggleCursorLock(gkState *state) {
    state->cursorLocked = !state->cursorLocked;
}

uint64_t gkFindTexture(gkState *state, const char *name) {
    uint64_t hash = MurmurHash((void*)name, strlen(name), 0);
    return imap_lookup(state->textureMap, hash) ? hash : -1L;
}

void gkPushTexture(gkState *state, uint64_t tid) {
    assert(state->textureStackCount < MAX_TEXTURE_STACK);
    assert(tid);
    imap_slot_t *slot = imap_lookup(state->textureMap, tid);
    assert(slot);
    state->textureStack[state->textureStackCount++] = tid;
    state->currentTexture = (gkTexture*)imap_getval64(state->textureMap, slot);
}

uint64_t gkPopTexture(gkState *state) {
    assert(state->textureStackCount > 0);
    uint64_t result = state->textureStack[state->textureStackCount-1];
    state->textureStack[state->textureStackCount--] = 0;
    return result;
}

void gkDrawTexture(gkState *state) {
    uint64_t tid = state->textureStack[state->textureStackCount-1];
    assert(tid && state->currentTexture);
    gkInternalDrawCall *call = malloc(sizeof(gkInternalDrawCall));
    call->texture = state->currentTexture;
//    memcpy(&call->desc, &state->currentDrawCall, sizeof(gkDrawCall));
    ezStackAppend(&state->commandQueue, GAMEKIT_DRAW_CALL_SINGLE, (void*)call);
}

void gkClear(gkState *state) {
    
}

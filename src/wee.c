//
//  wee.c
//  wee
//
//  Created by George Watson on 21/07/2023.
//


#include "wee.h"

static int CompareTextureID(const void *a, const void *b, void *udata) {
    const TextureBucket *ua = a;
    const TextureBucket *ub = b;
    return ua->tid == ub->tid ? 0 : 1;
}

static int CompareTextureName(const void *a, const void *b, void *udata) {
    const TextureBucket *ua = a;
    const TextureBucket *ub = b;
    return strcmp(ua->name, ub->name);
}

#if !defined(WEE_STATE)
#include "framebuffer.glsl.h"
#include "texture.glsl.h"

static Texture* NewTexture(sg_image_desc *desc) {
    Texture *result = malloc(sizeof(Texture));
    result->internal = sg_make_image(desc);
    result->w = desc->width;
    result->h = desc->height;
    return result;
}

static Texture* CreateEmptyTexture(unsigned int w, unsigned int h) {
    sg_image_desc desc = {
        .width = w,
        .height = h,
//        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .usage = SG_USAGE_STREAM
    };
    return NewTexture(&desc);
}

static void DestroyTexture(Texture *texture) {
    if (texture) {
        if (sg_query_image_state(texture->internal) == SG_RESOURCESTATE_VALID)
            sg_destroy_image(texture->internal);
        free(texture);
    }
}

static void UpdateTexture(Texture *texture, ezImage *img) {
    if (texture->w != img->w || texture->h != img->h) {
        DestroyTexture(texture);
        texture = CreateEmptyTexture(img->w, img->h);
    }
    sg_image_data data = {
        .subimage[0][0] = (sg_range) {
            .ptr = img->buf,
            .size = img->w * img->h * sizeof(int)
        }
    };
    sg_update_image(texture->internal, &data);
}

static Texture* LoadTextureFromImage(ezImage *img) {
    Texture *result = CreateEmptyTexture(img->w, img->h);
    UpdateTexture(result, img);
    return result;
}

static Texture* LoadTextureFromFile(const char *path) {
    ezImage *img = ezImageLoadFromPath(path);
    Texture *result = LoadTextureFromImage(img);
    ezImageFree(img);
    return result;
}

typedef Vertex Quad[6];

static void GenerateQuad(Vec2f position, Vec2f textureSize, Vec2f size, Vec2f scale, Vec2f viewportSize, float rotation, Rect clip, Quad *out) {
    Vec2f quad[4] = {
        {position.x, position.y + size.y}, // bottom left
        {position.x + size.x, position.y + size.y}, // bottom right
        {position.x + size.x, position.y }, // top right
        {position.x, position.y }, // top left
    };
    float vw =  2.f / (float)viewportSize.x;
    float vh = -2.f / (float)viewportSize.y;
    for (int j = 0; j < 4; j++)
        quad[j] = (Vec2f) {
            (vw * quad[j].x + -1.f) * scale.x,
            (vh * quad[j].y +  1.f) * scale.y
        };
    
    float iw = 1.f/textureSize.x, ih = 1.f/(float)textureSize.y;
    float tl = clip.x*iw;
    float tt = clip.y*ih;
    float tr = (clip.x + clip.w)*iw;
    float tb = (clip.y + clip.h)*ih;
    Vec2f vtexquad[4] = {
        {tl, tb}, // bottom left
        {tr, tb}, // bottom right
        {tr, tt}, // top right
        {tl, tt}, // top left
    };
    static int indices[6] = {
        0, 1, 2,
        3, 0, 2
    };
    
    for (int i = 0; i < 6; i++)
        (*out)[i] = (Vertex) {
            .position = quad[indices[i]],
            .texcoord = vtexquad[indices[i]],
            .color = {1.f, 1.f, 1.f, 1.f}
        };
}

static void DrawTexture(Texture *texture, Vec2f position, Vec2f size, Vec2f scale, Vec2f viewportSize, float rotation, Rect clip) {
    Quad quad;
    Vec2f textureSize = {texture->w, texture->h};
    if (clip.w < 0 && clip.h < 0) {
        clip.w = textureSize.x;
        clip.h = textureSize.y;
    }
    GenerateQuad(position, textureSize, size.x < 0 && size.y < 0 ? textureSize : size, scale, viewportSize, rotation, clip, &quad);
    sg_buffer_desc desc = {
        .data = SG_RANGE(quad)
    };
    sg_bindings bind = {
        .vertex_buffers[0] = sg_make_buffer(&desc),
        .fs_images[SLOT_tex] = texture->internal
    };
    sg_apply_bindings(&bind);
    sg_draw(0, 6, 1);
    sg_destroy_buffer(bind.vertex_buffers[0]);
}
#endif

static TextureBatch* EmptyTextureBatch(Texture *texture) {
    TextureBatch *result = malloc(sizeof(TextureBatch));
    memset(result, 0, sizeof(TextureBatch));
    result->size = (Vec2f){texture->w, texture->h};
    result->texture = texture;
    return result;
}

#if !defined(WEE_STATE)
static void CompileTextureBatch(TextureBatch *batch) {
    batch->vertices = malloc(batch->maxVertices * sizeof(Vertex));
    sg_buffer_desc desc = {
        .usage = SG_USAGE_STREAM,
        .size = batch->maxVertices * sizeof(Vertex)
    };
    batch->bind = (sg_bindings) {
        .vertex_buffers[0] = sg_make_buffer(&desc),
        .fs_images[SLOT_tex] = batch->texture->internal
    };
}
static void DestroyTextureBatch(TextureBatch *batch) {
    if (batch) {
        if (batch->vertices)
            free(batch->vertices);
        if (sg_query_buffer_state(batch->bind.vertex_buffers[0]) == SG_RESOURCESTATE_VALID)
            sg_destroy_buffer(batch->bind.vertex_buffers[0]);
        free(batch);
    }
}

static void TextureBatchDraw(TextureBatch *batch, Vec2f position, Vec2f size, Vec2f scale, Vec2f viewportSize, float rotation, Rect clip) {
    GenerateQuad(position, batch->size, size, scale, viewportSize, rotation, clip, (Quad*)(batch->vertices + batch->vertexCount));
    batch->vertexCount += 6;
}

static void FlushTextureBatch(TextureBatch *batch) {
    sg_range range = {
        .ptr = batch->vertices,
        .size = batch->vertexCount * sizeof(Vertex)
    };
    sg_update_buffer(batch->bind.vertex_buffers[0], &range);
    sg_apply_bindings(&batch->bind);
    sg_draw(0, batch->vertexCount, 1);
    memset(batch->vertices, 0, batch->maxVertices * sizeof(Vertex));
    batch->vertexCount = 0;
}

static uint64_t HashTexture(const void *item, uint64_t seed0, uint64_t seed1) {
    const TextureBucket *b = item;
    return b->name ? hashmap_sip(b->name, strlen(b->name), seed0, seed1) : b->tid;
}

static void FreeTexture(void *item) {
    TextureBucket *b = item;
    if (b)
        DestroyTexture(b->texture);
}


weeState state = {
    .running = false,
    .desc = (sapp_desc) {
#define X(NAME, TYPE, VAL, DEFAULT, DOCS) .VAL = DEFAULT,
        SETTINGS
#undef X
        .window_title = DEFAULT_WINDOW_TITLE
    },
    .pass_action = {
        .colors[0] = {.action=SG_ACTION_CLEAR, .value={0.39f, 0.58f, 0.92f, 1.f}}
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

static void InitCallback(void) {
    sg_desc desc = (sg_desc) {
        // TODO: Add more configuration options for sg_desc
        .context = sapp_sgcontext()
    };
    sg_setup(&desc);
    stm_setup();
    
    state.stateMap = hashmap_new(sizeof(SceneBucket), 0, 0, 0, HashScene, CompareScene, FreeScene, NULL);
    state.textureMap = hashmap_new(sizeof(TextureBucket), 0, 0, 0, HashTexture, NULL, FreeTexture, NULL);
   
    state.assets = ezContainerRead(WEE_ASSETS_PATH);
    for (int i = 0; i < state.assets->sizeOfEntries; i++) {
        ezContainerTreeEntry *e = &state.assets->entries[i];
        state.textureMap->compare = CompareTextureName;
        const char *ext = FileExt(e->filePath);
        if (!strncmp(ext, "png", 3)) {
            TextureBucket search = {.name = FileName(e->filePath)};
            TextureBucket *found = hashmap_get(state.textureMap, (void*)&search);
            assert(!found);
            search.texture = LoadTextureFromFile(e->filePath);
            search.tid = state.textureMap->hash((void*)&search, 0, 0) << 16 >> 16;;
            search.path = e->filePath;
            hashmap_set(state.textureMap, (void*)&search);
        }
    }
    
    state.drawCallStack.front = state.drawCallStack.back = NULL;
    
    sg_pipeline_desc offscreen_desc = {
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLES,
        .shader = sg_make_shader(texture_program_shader_desc(sg_query_backend())),
        .layout = {
            .buffers[0].stride = sizeof(Vertex),
            .attrs = {
                [ATTR_texture_vs_position].format=SG_VERTEXFORMAT_FLOAT2,
                [ATTR_texture_vs_texcoord].format=SG_VERTEXFORMAT_FLOAT2,
                [ATTR_texture_vs_color].format=SG_VERTEXFORMAT_FLOAT4
            }
        },
        .depth = {
//            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .colors[0] = {
            .blend = {
                .enabled = true,
                .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .op_rgb = SG_BLENDOP_ADD,
                .src_factor_alpha = SG_BLENDFACTOR_ONE,
                .dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .op_alpha = SG_BLENDOP_ADD
            },
//            .pixel_format = SG_PIXELFORMAT_RGBA8
        }
    };
    state.pip = sg_make_pipeline(&offscreen_desc);
    
    state.windowWidth = sapp_width();
    state.windowHeight = sapp_height();
    state.drawCallDesc = (weeDrawCallDesc) {
        .position = Vec2Zero(),
        .viewport = Vec2New((float)state.windowWidth, (float)state.windowHeight),
        .scale = Vec2New(1.f, 1.f),
        .clip = (Rect){0.f, 0.f, 0.f, 0.f},
        .rotation = 0.f
    };
    
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
    
    weeInit(&state);
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
    
    if (state.wis && state.wis->scene->preframe)
        state.wis->scene->preframe(&state, state.wis->context);
    
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
    
    sg_begin_default_pass(&state.pass_action, state.windowWidth, state.windowHeight);
    sg_apply_pipeline(state.pip);
    if (state.wis && state.wis->scene->frame)
        state.wis->scene->frame(&state, state.wis->context, render_time);
    
    ezStackEntry *callEntry = NULL;
    while ((callEntry = ezStackDrop(&state.drawCallStack))) {
        weeDrawCall *call = callEntry->data;
        Vec2f size = Vec2New((float)call->bucket->texture->w, (float)call->bucket->texture->h);
        switch (call->type) {
            case DRAW_CALL_SINGLE:
                if (call->desc.clip.x == 0.f && call->desc.clip.y == 0.f && call->desc.clip.w == 0.f && call->desc.clip.h == 0.f) {
                    call->desc.clip.w = size.x;
                    call->desc.clip.h = size.y;
                }
                DrawTexture(call->bucket->texture, call->desc.position, size, call->desc.scale, call->desc.viewport, call->desc.rotation, call->desc.clip);
                break;
            case DRAW_CALL_BATCH: {
                weeDrawCallDesc *cursor = call->desc.head;
                call->batch->maxVertices = call->desc.back->index * 6;
                CompileTextureBatch(call->batch);
                while (cursor) {
                    weeDrawCallDesc *tmp = cursor->next;
                    if (cursor->clip.x == 0.f && cursor->clip.y == 0.f && cursor->clip.w == 0.f && cursor->clip.h == 0.f) {
                        cursor->clip.w = size.x;
                        cursor->clip.h = size.y;
                    }
                    TextureBatchDraw(call->batch, cursor->position, size, cursor->scale, cursor->viewport, cursor->rotation, cursor->clip);
                    free(cursor);
                    cursor = tmp;
                }
                FlushTextureBatch(call->batch);
                DestroyTextureBatch(call->batch);
                state.drawCallDesc.head = state.drawCallDesc.back = state.drawCallDesc.next = NULL;
                break;
            }
            default:
                assert(0);
        }
        free(call);
        free(callEntry);
    }
    
    sg_end_pass();
    sg_commit();
    
    if (state.wis && state.wis->scene->postframe)
        state.wis->scene->postframe(&state, state.wis->context);
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
    sg_destroy_pipeline(state.pip);
    ezContainerFree(state.assets);
#define X(NAME) \
    weeDestroyScene(&state, NAME);
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
    
    state.desc.init_cb = InitCallback;
    state.desc.frame_cb = FrameCallback;
    state.desc.event_cb = EventCallback;
    state.desc.cleanup_cb = CleanupCallback;
    return state.desc;
}
#endif

void weeCreateScene(weeState *state, const char *name, const char *path) {
    SceneBucket search = {.name = name};
    SceneBucket *found =  hashmap_get(state->stateMap, (void*)&search);
    assert(!found);
    weeInternalScene *wis = malloc(sizeof(weeInternalScene));
    wis->path = path;
    wis->context = NULL;
    wis->scene = NULL;
    wis->handle = NULL;
    assert(ReloadLibrary(wis));
    search.wis = wis;
    hashmap_set(state->stateMap, (void*)&search);
}

void weePushScene(weeState *state, const char *name) {
    SceneBucket search = {.name = name};
    SceneBucket *found = hashmap_get(state->stateMap, (void*)&search);
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
    if ((found = hashmap_get(state->stateMap, (void*)&search)))
        hashmap_delete(state->stateMap, (void*)found);
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

uint64_t weeFindTexture(weeState *state, const char *name) {
    TextureBucket search = {.name = name};
    state->textureMap->compare = CompareTextureName;
    TextureBucket *found = hashmap_get(state->textureMap, (void*)&search);
    return found ? found->tid : 0;
}

void weeDrawTexture(weeState *state, uint64_t tid) {
    assert(tid);
    TextureBucket search = {.tid=tid};
    state->textureMap->compare = CompareTextureID;
    TextureBucket *found = hashmap_get(state->textureMap, (void*)&search);
    assert(found);
    weeDrawCall *call = malloc(sizeof(weeDrawCall));
    call->bucket = found;
    call->type = DRAW_CALL_SINGLE;
    memcpy(&call->desc, &state->drawCallDesc, sizeof(weeDrawCallDesc));
    ezStackAppend(&state->drawCallStack, 0, (void*)call);
}

void weeBeginBatch(weeState *state, uint64_t tid) {
    assert(!state->currentBatch);
    assert(tid);
    TextureBucket search = {.tid=tid};
    state->textureMap->compare = CompareTextureID;
    TextureBucket *found = hashmap_get(state->textureMap, (void*)&search);
    assert(found);
    state->currentTextureBucket = found;
    state->currentBatch = EmptyTextureBatch(found->texture);
}

void weeDrawTextureBatch(weeState *state) {
    assert(state->currentBatch);
    weeDrawCallDesc *node = malloc(sizeof(weeDrawCallDesc));
    node->next =  NULL;
    memcpy(node, &state->drawCallDesc, sizeof(weeDrawCallDesc));
    
    if (!state->drawCallDesc.head) {
        node->index = 1;
        state->drawCallDesc.head = state->drawCallDesc.back = node;
    } else {
        node->index = state->drawCallDesc.back->index + 1;
        state->drawCallDesc.back->next = node;
        state->drawCallDesc.back = node;
    }
}

void weeEndBatch(weeState *state) {
    assert(state->currentBatch);
    weeDrawCall *call = malloc(sizeof(weeDrawCall));
    call->bucket = state->currentTextureBucket;
    call->batch = state->currentBatch;
    call->type = DRAW_CALL_BATCH;
    memcpy(&call->desc, &state->drawCallDesc, sizeof(weeDrawCallDesc));
    ezStackAppend(&state->drawCallStack, 0, (void*)call);
    state->currentBatch = NULL;
    state->currentTextureBucket = NULL;
}

void weeSetPosition(weeState *state, float x, float y) {
    state->drawCallDesc.position = Vec2New(x, y);
}

void weePositionMoveBy(weeState *state, float dx, float dy) {
    state->drawCallDesc.position += Vec2New(dx, dy);
}

void weeSetScale(weeState *state, float x, float y) {
    state->drawCallDesc.scale = Vec2New(x, y);
}

void weeScaleBy(weeState *state, float dx, float dy) {
    state->drawCallDesc.scale += Vec2New(dx, dy);
}

void weeSetClip(weeState *state, float x, float y, float w, float h) {
    state->drawCallDesc.clip = (Rect){x, y, w, h};
}

void weeSetRotation(weeState *state, float angle) {
    state->drawCallDesc.rotation = angle;
}

void weeReset(weeState *state) {
    memset(&state->drawCallDesc, 0, sizeof(weeDrawCallDesc));
    state->drawCallDesc.viewport = Vec2New((float)state->windowWidth, (float)state->windowHeight);
}

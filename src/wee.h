/* wee.h -- https://github.com/takeiteasy/wee
 
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

#ifndef wee_h
#define wee_h
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @header wee
 * @copyright George Watson
 */

#define wee(F, ...) wee##F(__VA_ARGS__)
#define weee wee
#define weeee wee
#define weeeee wee
#define weeeeee wee
#define weeeeeee wee
#define weeeeeeee wee
#define weeeeeeeee wee
#define weeeeeeeeee wee
#define weeeeeeeeeee wee
#define weeeeeeeeeeee wee
#define weeeeeeeeeeeee wee
#define weeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee
#define weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee wee

#if defined(__EMSCRIPTEN__) || defined(EMSCRIPTEN)
#include <emscripten.h>
#define WEE_EMSCRIPTEN
#endif

#define WEE_POSIX
#if defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
#define WEE_MAC
#elif defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
#define WEE_WINDOWS
#if !defined(WEE_FORCE_POSIX)
#undef WEE_POSIX
#endif
#elif defined(__gnu_linux__) || defined(__linux__) || defined(__unix__)
#define WEE_LINUX
#else
#error "Unsupported operating system"
#endif

#if defined(WEE_WINDOWS) && !defined(WEE_NO_EXPORT)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

// Taken from: https://gist.github.com/61131/7a22ac46062ee292c2c8bd6d883d28de
#define N_ARGS(...) _NARG_(__VA_ARGS__, _RSEQ())
#define _NARG_(...) _SEQ(__VA_ARGS__)
#define _SEQ(_1, _2, _3, _4, _5, _6, _7, _8, _9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63,_64,_65,_66,_67,_68,_69,_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_80,_81,_82,_83,_84,_85,_86,_87,_88,_89,_90,_91,_92,_93,_94,_95,_96,_97,_98,_99,_100,_101,_102,_103,_104,_105,_106,_107,_108,_109,_110,_111,_112,_113,_114,_115,_116,_117,_118,_119,_120,_121,_122,_123,_124,_125,_126,_127,N,...) N
#define _RSEQ() 127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0


#if defined(_MSC_VER) && _MSC_VER < 1800
#include <windef.h>
#define bool BOOL
#define true 1
#define false 0
#else
#if defined(__STDC__) && __STDC_VERSION__ < 199901L
typedef enum bool { false = 0, true = !false } bool;
#else
#include <stdbool.h>
#endif
#endif
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// MARK: Math types

#define SIZES \
    X(2)      \
    X(3)      \
    X(4)

#define X(N)                                                          \
    typedef int pos##N __attribute__((ext_vector_type(N)));           \
    typedef float vec##N __attribute__((ext_vector_type(N)));         \
    vec##N Vec##N##Zero(void);                                        \
    float Vec##N##LengthSqr(vec##N v);                                \
    float Vec##N##Length(vec##N v);                                   \
    float Vec##N##Dot(vec##N v1, vec##N v2);                          \
    vec##N Vec##N##Normalize(vec##N v);
SIZES
#undef X
#define Vec2(X, Y) (vec2){(X), (Y)}
#define Vec3(X, Y, Z) (vec3){(X), (Y), (Z)}
#define Vec4(X, Y, Z, W) (vec4){(X), (Y), (Z), (W)}
typedef vec4 quat;
#define Quat(...) Vec4(__VA_ARGS__)

vec3 Vec3Cross(vec3 v1, vec3 v2);

// MARK: Event types

typedef enum {
    KEY_INVALID          = 0,
    KEY_SPACE            = 32,
    KEY_APOSTROPHE       = 39,  /* ' */
    KEY_COMMA            = 44,  /* , */
    KEY_MINUS            = 45,  /* - */
    KEY_PERIOD           = 46,  /* . */
    KEY_SLASH            = 47,  /* / */
    KEY_0                = 48,
    KEY_1                = 49,
    KEY_2                = 50,
    KEY_3                = 51,
    KEY_4                = 52,
    KEY_5                = 53,
    KEY_6                = 54,
    KEY_7                = 55,
    KEY_8                = 56,
    KEY_9                = 57,
    KEY_SEMICOLON        = 59,  /* ; */
    KEY_EQUAL            = 61,  /* = */
    KEY_A                = 65,
    KEY_B                = 66,
    KEY_C                = 67,
    KEY_D                = 68,
    KEY_E                = 69,
    KEY_F                = 70,
    KEY_G                = 71,
    KEY_H                = 72,
    KEY_I                = 73,
    KEY_J                = 74,
    KEY_K                = 75,
    KEY_L                = 76,
    KEY_M                = 77,
    KEY_N                = 78,
    KEY_O                = 79,
    KEY_P                = 80,
    KEY_Q                = 81,
    KEY_R                = 82,
    KEY_S                = 83,
    KEY_T                = 84,
    KEY_U                = 85,
    KEY_V                = 86,
    KEY_W                = 87,
    KEY_X                = 88,
    KEY_Y                = 89,
    KEY_Z                = 90,
    KEY_LEFT_BRACKET     = 91,  /* [ */
    KEY_BACKSLASH        = 92,  /* \ */
    KEY_RIGHT_BRACKET    = 93,  /* ] */
    KEY_GRAVE_ACCENT     = 96,  /* ` */
    KEY_WORLD_1          = 161, /* non-US #1 */
    KEY_WORLD_2          = 162, /* non-US #2 */
    KEY_ESCAPE           = 256,
    KEY_ENTER            = 257,
    KEY_TAB              = 258,
    KEY_BACKSPACE        = 259,
    KEY_INSERT           = 260,
    KEY_DELETE           = 261,
    KEY_RIGHT            = 262,
    KEY_LEFT             = 263,
    KEY_DOWN             = 264,
    KEY_UP               = 265,
    KEY_PAGE_UP          = 266,
    KEY_PAGE_DOWN        = 267,
    KEY_HOME             = 268,
    KEY_END              = 269,
    KEY_CAPS_LOCK        = 280,
    KEY_SCROLL_LOCK      = 281,
    KEY_NUM_LOCK         = 282,
    KEY_PRINT_SCREEN     = 283,
    KEY_PAUSE            = 284,
    KEY_F1               = 290,
    KEY_F2               = 291,
    KEY_F3               = 292,
    KEY_F4               = 293,
    KEY_F5               = 294,
    KEY_F6               = 295,
    KEY_F7               = 296,
    KEY_F8               = 297,
    KEY_F9               = 298,
    KEY_F10              = 299,
    KEY_F11              = 300,
    KEY_F12              = 301,
    KEY_F13              = 302,
    KEY_F14              = 303,
    KEY_F15              = 304,
    KEY_F16              = 305,
    KEY_F17              = 306,
    KEY_F18              = 307,
    KEY_F19              = 308,
    KEY_F20              = 309,
    KEY_F21              = 310,
    KEY_F22              = 311,
    KEY_F23              = 312,
    KEY_F24              = 313,
    KEY_F25              = 314,
    KEY_KP_0             = 320,
    KEY_KP_1             = 321,
    KEY_KP_2             = 322,
    KEY_KP_3             = 323,
    KEY_KP_4             = 324,
    KEY_KP_5             = 325,
    KEY_KP_6             = 326,
    KEY_KP_7             = 327,
    KEY_KP_8             = 328,
    KEY_KP_9             = 329,
    KEY_KP_DECIMAL       = 330,
    KEY_KP_DIVIDE        = 331,
    KEY_KP_MULTIPLY      = 332,
    KEY_KP_SUBTRACT      = 333,
    KEY_KP_ADD           = 334,
    KEY_KP_ENTER         = 335,
    KEY_KP_EQUAL         = 336,
    KEY_LEFT_SHIFT       = 340,
    KEY_LEFT_CONTROL     = 341,
    KEY_LEFT_ALT         = 342,
    KEY_LEFT_SUPER       = 343,
    KEY_RIGHT_SHIFT      = 344,
    KEY_RIGHT_CONTROL    = 345,
    KEY_RIGHT_ALT        = 346,
    KEY_RIGHT_SUPER      = 347,
    KEY_MENU             = 348,
} weeKey;

typedef enum {
    BUTTON_LEFT    = 0x0,
    BUTTON_RIGHT   = 0x1,
    BUTTON_MIDDLE  = 0x2,
    BUTTON_INVALID = 0x100,
} weeButton;

typedef enum {
    MODIFIER_SHIFT = 0x1,
    MODIFIER_CTRL  = 0x2,
    MODIFIER_ALT   = 0x4,
    MODIFIER_SUPER = 0x8,
    MODIFIER_LMB   = 0x100,
    MODIFIER_RMB   = 0x200,
    MODIFIER_MMB   = 0x400
} weeModifier;

#define WEE_CALLBACKS           \
X(Init,         (void*))        \
X(Frame,        (void*, float)) \
X(Cleanup,      (void*))

// MARK: Setup functions

EXPORT void weeSetUserdata(void *userdata);
#define X(NAME, ARGS) void(*NAME##Callback)ARGS,
EXPORT void weeSetCallbacks(WEE_CALLBACKS void *userdata);
#undef X
#define X(NAME, ARGS) \
    EXPORT void weeSet##NAME##Callback(void(*NAME##Callback)ARGS);
WEE_CALLBACKS
#undef X
EXPORT int weeInit(int argc, const char *argv[]);

// MARK: Event functions

EXPORT bool IsKeyDown(weeKey key);
EXPORT bool IsKeyUp(weeKey key);
EXPORT bool WasKeyClicked(weeKey key);
EXPORT bool IsButtonDown(weeButton button);
EXPORT bool IsButtonUp(weeButton button);
EXPORT bool WasButtonPressed(weeButton button);

// MARK: Window management functions

EXPORT int weeWindowWidth(void);
EXPORT int weeWindowHeight(void);
EXPORT int weeIsWindowFullscreen(void);
EXPORT void weeToggleFullscreen(void);
EXPORT int weeIsCursorVisible(void);
EXPORT void weeToggleCursorVisible(void);
EXPORT int weeIsCursorLocked(void);
EXPORT void weeToggleCursorLock(void);

// MARK: ECS type

typedef union {
    struct {
        uint32_t id;
        uint16_t version;
        uint8_t unused;
        uint8_t flag;
    } parts;
    uint64_t id;
} Entity;

#define ENTITY_ID(E) \
    ((E).parts.id)
#define ENTITY_VERSION(E) \
    ((E).parts.version)
#define ENTITY_IS_NIL(E) \
    ((E).parts.id == EcsNil)
#define ENTITY_CMP(A, B) \
    ((A).id == (B).id)

#define EcsNil 0xFFFFFFFFull
#define EcsNilEntity (Entity){.id = EcsNil}

#define ECS_COMPONENT(T) EcsNewComponent(sizeof(T))
#define ECS_TAG(WORLD) EcsNewComponent(0)
#define ECS_QUERY(CB, UD, ...) EcsQuery(CB, UD, (Entity[]){__VA_ARGS__}, sizeof((Entity[]){__VA_ARGS__}) / sizeof(Entity));
#define ECS_FIELD(QUERY, T, IDX) (T*)EcsQueryField(QUERY, IDX)
#define ECS_SYSTEM(CB, ...) EcsNewSystem(CB, N_ARGS(__VA_ARGS__), __VA_ARGS__)
#define ECS_PREFAB(...) EcsNewPrefab(N_ARGS(__VA_ARGS__), __VA_ARGS__)
#define ECS_CHILDREN(PARENT, CB) (EcsRelations((PARENT), EcsChildof, (CB)))
#define ENTITY_ISA(E, TYPE) ((E).parts.flag == Ecs##TYPE##Type)

typedef struct {
    void **componentData;
    Entity *componentIndex;
    size_t sizeOfComponentData;
    Entity entity;
    void *userdata;
} Query;

typedef void(*SystemCb)(Query *query);

typedef struct {
    Entity *components;
    size_t sizeOfComponents;
} Prefab;

typedef struct {
    SystemCb callback;
    Entity *components;
    size_t sizeOfComponents;
    bool enabled;
} System;

typedef struct {
    Entity object,
           relation;
} Relation;

typedef void(*TimerCb)(void*);

typedef struct {
    uint64_t start;
    int interval;
    TimerCb cb;
    void *userdata;
    bool enabled;
} Timer;

typedef enum {
    EcsEntityType    = 0,
    EcsComponentType = (1 << 0),
    EcsSystemType    = (1 << 1),
    EcsPrefabType    = (1 << 2),
    EcsRelationType  = (1 << 3),
    EcsTimerType     = (1 << 4)
} EntityFlag;

// MARK: ECS Functions

EXPORT Entity EcsNewEntity(void);
EXPORT Entity EcsNewComponent(size_t sizeOfComponent);
EXPORT Entity EcsNewSystem(SystemCb fn, size_t sizeOfComponents, ...);
EXPORT Entity EcsNewPrefab(size_t sizeOfComponents, ...);
EXPORT Entity EcsNewTimer(int interval, bool enable, TimerCb cb, void *userdata);
EXPORT void DestroyEntity(Entity entity);

EXPORT bool EcsIsValid(Entity entity);
EXPORT bool EcsHas(Entity entity, Entity component);
EXPORT void EcsAttach(Entity entity, Entity component);
EXPORT void EcsAssociate(Entity entity, Entity object, Entity relation);
EXPORT void EcsDetach(Entity entity, Entity component);
EXPORT void EcsDisassociate(Entity entity);
EXPORT bool EcsHasRelation(Entity entity, Entity object);
EXPORT bool EcsRelated(Entity entity, Entity relation);
EXPORT void* EcsGet(Entity entity, Entity component);
EXPORT void EcsSet(Entity entity, Entity component, const void *data);
EXPORT void EcsRelations(Entity parent, Entity relation, void *userdata, SystemCb cb);

EXPORT void EcsEnableSystem(Entity system);
EXPORT void EcsDisableSystem(Entity system);
EXPORT void EcsEnableTimer(Entity timer);
EXPORT void EcsDisableTimer(Entity timer);

EXPORT void EcsRunSystem(Entity system);
EXPORT void EcsStep(void);
EXPORT void EcsQuery(SystemCb cb, void *userdata, Entity *components, size_t sizeOfComponents);
EXPORT void* EcsQueryField(Query *query, size_t index);

EXPORT extern Entity EcsSystem;
EXPORT extern Entity EcsPrefab;
EXPORT extern Entity EcsRelation;
EXPORT extern Entity EcsChildOf;
EXPORT extern Entity EcsTimer;

#if defined(__cplusplus)
}
#endif
#endif // wee_h

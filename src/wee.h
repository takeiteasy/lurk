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

#if defined(__cplusplus)
}
#endif
#endif // wee_h

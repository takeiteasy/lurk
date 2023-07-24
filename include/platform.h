//
//  platform.h
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#ifndef platform_h
#define platform_h

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

#endif /* platform_h */

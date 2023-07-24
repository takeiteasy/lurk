//
//  filesystem.h
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#ifndef filesystem_h
#define filesystem_h
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#define F_OK    0
#define access _access
#define PATH_SEPERATOR "\\"
#else
#include <unistd.h>
#define PATH_SEPERATOR "/"
#endif

#define SAVE_PATH "worlds"
#define Path(FMT, ...) FormatString("%s%s" FMT, SAVE_PATH, PATH_SEPERATOR, __VA_ARGS__)

bool DoesFileExist(const char *path);
bool DoesDirExist(const char *path);
char* FormatString(const char *fmt, ...);
char* LoadFile(const char *path, size_t *length);

#endif /* filesystem_h */

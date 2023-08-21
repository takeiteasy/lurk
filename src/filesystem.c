//
//  filesystem.c
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#include "wee.h"

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

static const char *PathSeperator(void) {
    static char c = PATH_SEPERATOR;
    char *seperator = (char*)&c;
    seperator[1] = '\0';
    return seperator;
}

const char* JoinPath(const char *a, const char *b) {
    static char buf[MAX_PATH];
    memset(buf, 0, MAX_PATH * sizeof(char));
    strcat(buf, a);
    if (b) {
        if (a[strlen(a)-1] != PATH_SEPERATOR && b[0] != PATH_SEPERATOR)
            strcat(buf, PathSeperator());
        strcat(buf, b);
    } else
        strcat(buf, PathSeperator());
    return buf;
}

const char* UserPath(void) {
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

EXPORT const char* CurrentDirectory(void) {
    static char buf[MAX_PATH];
    memset(buf, 0, MAX_PATH * sizeof(char));
    getcwd(buf, MAX_PATH);
    size_t length = strlen(buf);
    if (buf[length-1] != PATH_SEPERATOR)
        buf[length] = PATH_SEPERATOR;
    return buf;
}

EXPORT bool SetCurrentDirectory(const char *path) {
    const char *rpath = ResolvePath(path);
    if (DoesDirExist(rpath)) {
        chdir(rpath);
        return true;
    }
    return false;
}

static int PathDepth(const char *path) {
    assert(DoesDirExist(path));
#if defined(WEE_WINDOWS)
    
#else
    assert(path && path[0] == '/');
    const char *cursor = path + 1;
    int depth = 0;
    char last = '\0';
    while (cursor) {
        last = cursor[0];
        if (last == PATH_SEPERATOR)
            depth++;
        cursor++;
    }
    if (last != PATH_SEPERATOR)
        depth++;
#endif
    return depth;
}

static const char* PathParent(const char *path, int parent) {
    static char buf[MAX_PATH];
    memcpy(buf, 0, MAX_PATH * sizeof(char));
    int depth = PathDepth(path);
    assert(parent < depth);
    const char *cursor = path + 1;
    int length = 0;
    for (int remainder = depth - parent; remainder > 0; remainder--) {
        bool waiting = true;
        while (waiting) {
            if (cursor[0] == PATH_SEPERATOR)
                waiting = false;
            cursor++;
            length++;
        }
    }
    memcpy(buf, buf, length * sizeof(char));
    return buf;
}

const char* ResolvePath(const char *path) {
    assert(path);
    size_t pathSize = strlen(path);
    switch (path[0]) {
        case '~':
            assert(pathSize > 2);
            return path[1] == PATH_SEPERATOR ? JoinPath(UserPath(), path + 2) : path;
        case '.':
            if (pathSize == 1)
                return CurrentDirectory();
            switch (path[1]) {
                case '.':
                    switch (pathSize) {
                        case 2:
                            return PathParent(path, 1);
                        case 3:
                            if (path[2] == PATH_SEPERATOR)
                                return PathParent(path, 1);
                            break;
                        default:
                            assert(pathSize > 3);
                            assert(path[2] == PATH_SEPERATOR);
                            static char back[3] = { '.', '.', PATH_SEPERATOR };
                            const char *cursor = path + 3;
                            int backtrack = 1;
                            while (cursor) {
                                if (strlen(cursor) < 3 || strncmp(cursor, back, 3))
                                    return JoinPath(PathParent(path, backtrack), cursor);
                                cursor += 3;
                                backtrack++;
                            }
                            return PathParent(path, backtrack);
                    }
                    break;
                case PATH_SEPERATOR:
                default:
                    break;
            }
            break;
    }
    return JoinPath(CurrentDirectory(), path);
}

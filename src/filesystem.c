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

const char* JoinPath(const char *a, const char *b) {
    static char buffer[MAX_PATH];
    buffer[0] = '\0';
    strcat(buffer, a);
    if (a[strlen(a)-1] != PATH_SEPERATOR && b[0] != PATH_SEPERATOR) {
        static char c = PATH_SEPERATOR;
        char *seperator = (char*)&c;
        seperator[1] = '\0';
        strcat(buffer, seperator);
    }
    strcat(buffer, b);
//    buffer[strlen(buffer)] = '\0';
    return buffer;
}

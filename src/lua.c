//
//  lua.c
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#define LUA_IMPL
#include "lua.h"

void LuaDumpTable(lua_State* L, int idx) {
    printf("--------------- LUA TABLE DUMP ---------------\n");
    lua_pushvalue(L, idx);
    lua_pushnil(L);
    int t, j = (idx < 0 ? -idx : idx), i = idx - 1;
    const char *key = NULL, *tmp = NULL;
    while ((t = lua_next(L, i))) {
        lua_pushvalue(L, idx - 1);
        key = lua_tostring(L, idx);
        switch (lua_type(L, idx - 1)) {
            case LUA_TSTRING:
                printf("%s (string, %d) => `%s'\n", key, j, lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf("%s (boolean, %d) => %s\n", key, j, lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("%s (integer, %d) => %g\n", key, j, lua_tonumber(L, i));
                break;
            default:
                tmp = lua_typename(L, i);
                printf("%s (%s, %d) => %s\n", key, tmp, j, tmp);
                if (!strncmp(lua_typename(L, t), "table", 5))
                    LuaDumpTable(L, i);
                break;
        }
        lua_pop(L, 2);
    }
    lua_pop(L, 1);
    printf("--------------- END TABLE DUMP ---------------\n");
}

int LuaDumpStack(lua_State* L) {
    int t, i = lua_gettop(L);
    const char* tmp = NULL;
    printf("--------------- LUA STACK DUMP ---------------\n");
    for (; i; --i) {
        
        switch ((t = lua_type(L, i))) {
            case LUA_TSTRING:
                printf("%d (string): `%s'\n", i, lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf("%d (boolean): %s\n", i, lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("%d (integer): %g\n",  i, lua_tonumber(L, i));
                break;
            default:
                tmp = lua_typename(L, t);
                printf("%d (%s): %s\n", i, lua_typename(L, t), tmp);
                break;
        }
    }
    printf("--------------- END STACK DUMP ---------------\n");
    return 0;
}

void LuaFail(lua_State *L, char *msg, bool die) {
    LuaDumpStack(L);
    fprintf(stderr, "\nERROR:\n  %s: %s\n\n", msg, lua_tostring(L, -1));
    if (die)
        exit(1);
}

lua_State* LoadLuaScript(const char *filename) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    
    lua_pushcfunction(L, LuaDumpStack);
    lua_setglobal(L, "dump_stack");
    
    char asset[1024];
    sprintf(asset, "assets%s%s", PATH_SEPERATOR, filename);
    if (luaL_dofile(L, asset))
        LuaFail(L, "Errors found in lua script", false);
    return L;
}

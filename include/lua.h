//
//  lua.h
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#ifndef llua_h
#define llua_h
#include "minilua.h"
#include "filesystem.h"
#include "maths.h"

void LuaDumpTable(lua_State *L, int table_idx);
int LuaDumpStack(lua_State *L);
void LuaFail(lua_State *L, char *msg, bool die);
int LuaSettings(lua_State *L);
int LuaDelta(lua_State *L);
lua_State* LoadLuaScript(const char *filename);

#endif /* llua_h */

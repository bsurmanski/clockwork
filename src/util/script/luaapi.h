/**
 * luaapi.h
 * @file    lua_api.h
 * obj
 * @date    July 18, 2012
 * @author  Brandon Surmanski
 */

#ifndef _LUA_API_H
#define _LUA_API_H

#include <lua.h>
#include <stdint.h>

struct luaL_Reg;
struct lua_State;

typedef struct api_func
{
    char *name;
    int (*func)(lua_State *s); 
} api_func;

typedef struct api_const
{
    char *name; 
    int lua_type;
    uint64_t val;
} api_const;
    
typedef struct api_t {
    api_func *funcs;
    api_const *consts;
} api_t;

#define luaapi_setconst(l, name) {lua_pushnumber(l, name); lua_setglobal(l, #name);}

void *luaapi_alloc(void *ud, void *ptr, size_t osize, size_t nsize);
const char *luaapi_readfile(lua_State *l, void *data, size_t *size);
int luaapi_panic(lua_State *l);
void luaapi_openlibs(lua_State *l);
void luaapi_loadfile(lua_State *l, char *name, char *chunknm);

void luaapi_register(lua_State *l, const char *nm, struct api_t *funcs);

#endif

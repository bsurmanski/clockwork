/**
 * api.c
 * @file    api.h
 * obj
 * @date    July 18, 2012
 * @author  Brandon Surmanski
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "luaapi.h"

#define BUF_SZ 512

static int l_memsz = 0;

struct l_readstate 
{
    size_t bufsz;
    char *buf;
    FILE *file;
};

void *luaapi_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
    (void) ud; // not used
    l_memsz += (nsize - osize); 

    if(nsize == 0) 
    {
        free(ptr);
        return NULL;
    } 
    return realloc(ptr, nsize);
}

const char *luaapi_readfile(lua_State *l, void *data, size_t *size)
{
    struct l_readstate *state = (struct l_readstate*)data;
    *size = fread(state->buf, sizeof(char), state->bufsz, state->file);
    return state->buf;
}

int luaapi_panic(lua_State *l)
{
    lua_Debug ar;
    lua_getstack(l, 0, &ar);
    lua_getinfo(l, ">nSl", &ar);
    printf("Lua panic: %s:%d, %s\n", lua_tostring(l, lua_gettop(l)), ar.currentline, ar.source);
    return 0;
}


void luaapi_openlibs(lua_State *l)
{
#ifdef _WIN32
    luaL_openlibs(l);
#else
    
    lua_pushcfunction(l, luaopen_base);
    lua_pushcfunction(l, luaopen_table);
    lua_pushcfunction(l, luaopen_os);
    lua_pushcfunction(l, luaopen_string);
    lua_pushcfunction(l, luaopen_math);
    lua_pushcfunction(l, luaopen_package);
    lua_pushcfunction(l, luaopen_io);
    lua_call(l, 0, 0);
    lua_call(l, 0, 0);
    lua_call(l, 0, 0);
    lua_call(l, 0, 0);
    lua_call(l, 0, 0);
    lua_call(l, 0, 0);
    lua_call(l, 0, 0);
#endif
}

void luaapi_loadfile(lua_State *l, char *filenm, char *chunknm)
{
    char buf[BUF_SZ];
    FILE *file = fopen(filenm, "rb");
    struct l_readstate rstate = {BUF_SZ, buf, file};

    if(file)
    {

#if LUA_VERSION_NUM == 501
        int err = lua_load(l, luaapi_readfile, &rstate, chunknm);
#elif LUA_VERSION_NUM == 502
        int err = lua_load(l, luaapi_readfile, &rstate, chunknm, "bt");
#else 
#error Unknown or unsupported Lua distribution (Must be v5.1 or v5.2)
#endif

        if(err)
        {
            printf("Lua load error: %s\n", filenm);
            exit(-1); //TODO: error handle
        }
        fclose(file); 
    } else 
    {
        printf("Lua file not found: %s\n", filenm);
        exit(-1); //TODO: error handle
    }
}

void luaapi_register(lua_State *l, const char *api_nm, struct api_t *api)
{
    int i = 0;
    lua_newtable(l);
    int tableindex = lua_gettop(l);

    api_func *funcs = api->funcs;
    while(funcs && funcs->name)
    {
        lua_pushstring(l, funcs->name);
        lua_pushcfunction(l, funcs->func);
        lua_settable(l, tableindex);
        funcs++;
    }
    
    api_const *consts = api->consts;
    while(consts && consts->name)
    {
        lua_pushstring(l, consts->name);
        switch(consts->lua_type)
        {
            case LUA_TNUMBER:
                lua_pushnumber(l, consts->val);
                break;
            case LUA_TBOOLEAN:
                lua_pushboolean(l, (bool) consts->val);
                break;
            case LUA_TSTRING:
                lua_pushstring(l, (const char*) consts->val);
                break;
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA:
                lua_pushlightuserdata(l, (void*) consts->val);
                break;
            default:
                lua_pop(l, 1);
                continue;
        }
        lua_settable(l, tableindex);
        consts++;
    }
    lua_setglobal(l, api_nm);
}

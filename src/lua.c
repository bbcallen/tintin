/******************************************************************************
*   This file is part of TinTin++                                             *
*                                                                             *
*   Copyright 2004-2019 Igor van den Hoven                                    *
*   Copyright 2022 Zhang Rui <bbcallen@gmail.com                              *
*                                                                             *
*   TinTin++ is free software; you can redistribute it and/or modify          *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 3 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with TinTin++.  If not, see https://www.gnu.org/licenses.           *
******************************************************************************/

#include "tintin.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/*
    lua_pushcfunction(L, ngx_http_lua_ngx_quote_sql_str);
    lua_setfield(L, -2, "quote_sql_str");
*/
static const char *s_session_field_name = "#tt";
static const char *s_api_table_name = "tt";

#define TINTIN_LUA_MAX_BUFFER_BLOCKS (2)
#define TINTIN_LUA_MAX_BUFSIZE (BUFFER_SIZE)

typedef struct tt_lua_context {
    lua_State *vm;
    char *buffer[TINTIN_LUA_MAX_BUFFER_BLOCKS];
} tt_lua_context;

inline static void safe_strcpy(char *dst, const char *src, size_t bufsize, size_t len)
{
    strncpy(dst, src, (UMIN(len, bufsize)));
    dst[UMIN(len, (bufsize - 1))] = '\0';
}

static struct session *get_session(lua_State *vm)
{
    struct session *ses;
    
    lua_getglobal(vm, s_session_field_name);
    if (!lua_islightuserdata(vm, -1))
    {
        return NULL;
    }
    ses = (struct session *)lua_topointer(vm, -1);
    lua_pop(vm, 1);
    return ses;
}

static void set_session(lua_State *vm, struct session *ses)
{
    lua_pushlightuserdata(vm, ses);
    lua_setglobal(vm, s_session_field_name);
}

static int lua_pcall_error_func(lua_State *vm)
{
    lua_getglobal(vm, "debug");
    lua_getfield(vm, -1, "traceback");
    lua_pushvalue(vm, 1);
    lua_pushinteger(vm, 2);
    lua_call(vm, 2, 1);
    return 1;
}

static int l_print(lua_State *vm)
{
    struct session *ses;
    const char *par1;
    size_t len;
    char *buffer;

    ses = get_session(vm);
    if (!ses) {return luaL_error(vm, "l_print: NULL session access.\n");}

    len = 0;
    par1 = lua_tolstring(vm, 1, &len);
    if (!par1) {return luaL_typerror(vm, 1, "string");}

    buffer = ses->lua->buffer[0];
    safe_strcpy(buffer, par1, TINTIN_LUA_MAX_BUFSIZE, len);

    tintin_puts2(ses, buffer);
    return 0;
}

static int l_send(lua_State *vm)
{
    struct session *ses;
    const char *par1;
    size_t len;
    char *buffer;

    ses = get_session(vm);
    if (!ses) {return luaL_error(vm, "NULL session access.\n");}

    len = 0;
    par1 = lua_tolstring(vm, 1, &len);
    if (!par1) {return luaL_typerror(vm, 1, "string");}

    buffer = ses->lua->buffer[0];
    safe_strcpy(buffer, par1, TINTIN_LUA_MAX_BUFSIZE, len);

    script_driver(ses, LIST_COMMAND, buffer);
    return 0;
}

static int init_lua(struct session *ses)
{
    lua_State *vm;

    if (ses->lua)
    {
        return 0;
    }

    ses->lua = calloc(1, sizeof(tt_lua_context));
    if (!ses->lua)
    {
        show_error(ses, LIST_COMMAND, "\e[1;31m#ERROR: init_lua: out of memory");
        return -1;
    }

    ses->lua->buffer[0] = calloc(1, TINTIN_LUA_MAX_BUFSIZE);
    if (!ses->lua->buffer[0])
    {
        show_error(ses, LIST_COMMAND, "\e[1;31m#ERROR: init_lua: out of memory");
        free(ses->lua);
        ses->lua = NULL;
        return -1;
    }

    vm = lua_open();
    luaL_openlibs(vm);

    set_session(vm, ses);

    lua_newtable(vm);
    lua_pushcfunction(vm, l_print);
    lua_setfield(vm, -2, "print");
    lua_pushcfunction(vm, l_send);
    lua_setfield(vm, -2, "send");
    lua_setglobal(vm, s_api_table_name);

    ses->lua->vm = vm;
    return 0;
}

void free_lua(struct session *ses)
{
    int i;

    if (!ses->lua)
    {
        return;
    }

    for (i = 0; i < TINTIN_LUA_MAX_BUFFER_BLOCKS; ++i)
    {
        if (ses->lua->buffer[i])
        {
            free(ses->lua->buffer[i]);
            ses->lua->buffer[i] = NULL;
        }
    }

    lua_close(ses->lua->vm);
    ses->lua = NULL;
}

DO_COMMAND(do_lua)
{
    lua_State *vm;
    int top;

    arg = get_arg_in_braces(ses, arg, arg1, GET_ALL);

    substitute(ses, arg1, arg1, SUB_VAR|SUB_FUN);
    substitute(ses, arg1, arg1, SUB_COL|SUB_ESC);

    if (!ses->lua)
    {
        if (init_lua(ses))
        {
            show_error(ses, LIST_COMMAND, "\e[1;31m#ERROR: #LUA {%s} failed to init lua VM.", arg1);
            return ses;
        }
    }
    vm = ses->lua->vm;

    lua_pushcfunction(vm, lua_pcall_error_func);
    top = lua_gettop(vm);
    if (luaL_loadstring(vm, arg1))
    {
        show_error(ses, LIST_COMMAND, "\e[1;31m#ERROR: #LUA {%s} failed to load: %s", arg1, lua_tostring(vm, -1));
        lua_pop(vm, 1);
        return ses;
    }
    if (lua_pcall(vm, 0, 0, top))
    {
        show_error(ses, LIST_COMMAND, "\e[1;31m#ERROR: #LUA {%s} failed to call: %s", arg1, lua_tostring(vm, -1));
        lua_pop(vm, 1);
        return ses;
    }
    lua_pop(vm, 1);
	return ses;
}

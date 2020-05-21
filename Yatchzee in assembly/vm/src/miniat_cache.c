#include <errno.h>
#include <stdbool.h>

#include "miniat/miniat_error.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "miniat_cache.h"
#include "miniat_defines.h"
#include "miniat_memory.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"

#define LUA_CACHE_FN_NAME "cache_access"

static unsigned lua_cache_call(lua_State *L, bool is_ins, bool is_write,
		m_uword address) {

	/* Is it enabled? If not, return < 0 for default stall */
	if (!L) {
		return -1;
	}

	lua_getglobal(L, LUA_CACHE_FN_NAME);
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 1);
		THROW(EFAULT, "Could not find a function \"%s\" in lua script",
				LUA_CACHE_FN_NAME);
	}

	lua_pushboolean(L, is_ins);
	lua_pushboolean(L, is_write);

	/*
	 * Values and Types states that Lua by default uses
	 * 64 bit integers and 64 bit floats, and converts between
	 * the two as needed. Thus miniat words will always fit.
	 * See: https://www.lua.org/manual/5.3/manual.html
	 */
	lua_pushinteger(L, address);

	/*
	 * call lua cache_access(is_instruction, is_write, address)
	 */
	int rc = lua_pcall(L, 3, 1, 0);
	if (rc != LUA_OK) {
		/* pop the error message pcall balanced the pushed values thusfar */
		lua_pop(L, 1);
		THROW(EFAULT, "Error on pcall of lua function: \"%s\"",
				LUA_CACHE_FN_NAME);
	}

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);
		THROW(EINVAL, "Return from lua function \"%s\" is not a number.",
				LUA_CACHE_FN_NAME);
	}

	int stall = lua_tointeger(L, -1);

	/* balance the stack */
	lua_pop(L, 1);

	if (stall < 0) {
		THROW(EINVAL, "Return from lua function \"%s\" is negative, must be >= 0.",
				LUA_CACHE_FN_NAME);
	}

	return stall;
}

void m_cache_init(miniat *m, const char *lua_cache_file) {

	if (!lua_cache_file) {
		return;
	}

	lua_State *L = luaL_newstate();
	if (!L) {
		THROW(ENOMEM, "Could not initialize Lua, out of memory.");
	}

	m->cache.state = L;

	luaL_openlibs(L);

	int rc = luaL_dofile(L, lua_cache_file);
	if (rc != LUA_OK) {
		THROW(EBADF, "Could not execute lua script, possibly malformed.");
	}
}

unsigned m_cache_read(miniat *m, bool is_ins, m_uword address) {

	return lua_cache_call(m->cache.state, is_ins, false, address);
}

unsigned m_cache_write(miniat *m, bool is_ins, m_uword address) {

	return lua_cache_call(m->cache.state, is_ins, true, address);
}

void m_cache_destroy(miniat *m) {

	if (m->cache.state) {
		lua_close(m->cache.state);
	}
	m->cache.state = NULL;

	return;
}

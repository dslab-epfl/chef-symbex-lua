
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#ifdef LUA_SYMBEX
#include "s2e.h"


typedef enum {
	CONCOLIC_RET_OK,
	CONCOLIC_RET_TOO_SMALL,
	CONCOLIC_RET_ERROR
} ConcolicStatus;


typedef enum {
	START_CONCOLIC_SESSION,
	END_CONCOLIC_SESSION,
	GET_CONCOLIC_ALTERNATES
} ConcolicCommand;


typedef struct {
	ConcolicCommand command;
	uint32_t max_time;
	uint8_t is_error_path;
	uint32_t result_ptr;
	uint32_t result_size;
} __attribute__((packed)) ConcolicMessage;


static int symbex_symstring(lua_State *L) {
	size_t len;
	luaL_Buffer b;
	const char *value = luaL_checklstring(L, 1, &len);
	const char *name = luaL_checkstring(L, 2);

	luaL_buffinitsize(L, &b, len);
	luaL_addlstring(&b, value, len);

	s2e_make_concolic(b.b, b.n, name);

	luaL_pushresult(&b);
	return 1;
}


static int symbex_symint(lua_State *L) {
	lua_Number value = luaL_checknumber(L, 1);
	const char *name = luaL_checkstring(L, 2);

	s2e_make_concolic(&value, sizeof(value), name);

	lua_pushnumber(L, value);
	return 1;
}


static int symbex_killstate(lua_State *L) {
	int status = luaL_optinteger(L, 1, 0);
	const char *message = luaL_optstring(L, 2, "terminated by lua");

	s2e_kill_state(status, message);
	return 0;
}


static int symbex_issymbolic(lua_State *L) {
	if (lua_type(L, 1) == LUA_TSTRING) {
		size_t len;
		const char *value = lua_tolstring(L, 1, &len);
		lua_pushboolean(L,
				s2e_is_symbolic(&len, sizeof(len)) ||
				s2e_is_symbolic(&value, sizeof(value)) ||
				s2e_is_symbolic(value, len));
	} else if (lua_type(L, 1) == LUA_TNUMBER) {
		lua_Number value = lua_tonumber(L, 1);
		lua_pushboolean(L,
				s2e_is_symbolic(&value, sizeof(value)));
	} else {
		lua_pushboolean(L, 0);
	}

	return 1;
}


static int symbex_startconcolic(lua_State *L) {
	ConcolicMessage message;
	int max_time = luaL_checkint(L, 1);

	memset(&message, 0, sizeof(message));
	message.command = START_CONCOLIC_SESSION;
	message.max_time = max_time;

	lua_pushinteger(L, s2e_invoke_plugin("ConcolicSession",
			(void*)&message, sizeof(message)));
	return 1;
}


static int symbex_endconcolic(lua_State *L) {
	ConcolicMessage message;
	int is_error_path = lua_toboolean(L, 1);

	memset(&message, 0, sizeof(message));
	message.command = END_CONCOLIC_SESSION;
	message.is_error_path = is_error_path;

	lua_pushinteger(L, s2e_invoke_plugin("ConcolicSession",
			(void*)&message, sizeof(message)));
	return 1;
}


#else

#define NOT_IN_SYMBEX(fn_name) \
	static int fn_name(lua_State *L) { \
		return luaL_error(L, "Not built with symbolic execution support"); \
    }

NOT_IN_SYMBEX(symbex_symstring)

NOT_IN_SYMBEX(symbex_symint)

NOT_IN_SYMBEX(symbex_killstate)

NOT_IN_SYMBEX(symbex_issymbolic)

NOT_IN_SYMBEX(symbex_startconcolic)

NOT_IN_SYMBEX(symbex_endconcolic)

#endif /* LUA_SYMBEX */


static const luaL_Reg symbexlib[] = {
	{ "symstring", symbex_symstring },
	{ "symint", symbex_symint },
	{ "killstate", symbex_killstate },
	{ "issymbolic", symbex_issymbolic },
	{ "startconcolic", symbex_startconcolic },
	{ "endconcolic", symbex_endconcolic },
	{ NULL, NULL }
};


LUAMOD_API int luaopen_symbex(lua_State *L) {
	luaL_newlib(L, symbexlib);
	return 1;
}

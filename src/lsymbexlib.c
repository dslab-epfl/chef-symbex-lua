
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "chef.h"


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
	lua_Number min_value = luaL_optnumber(L, 3, 0);
	lua_Number max_value = luaL_optnumber(L, 4, 0);

	if (min_value > max_value) {
	    return luaL_error(L, "invalid numeric range for symbolic integer");
	}

	s2e_make_concolic(&value, sizeof(value), name);

    if (min_value < max_value) {
        s2e_assume(value >= min_value);
        s2e_assume(value <= max_value);
    }

	lua_pushnumber(L, value);
	return 1;
}


static int symbex_killstate(lua_State *L) {
	int status = luaL_optinteger(L, 1, 0);
	const char *message = luaL_optstring(L, 2, "terminated");

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


static int symbex_calibrate(lua_State *L) {
    lua_Number op = luaL_checknumber(L, 1);
    lua_Number increment = luaL_optnumber(L, 2, 0);

    enum {
        S2E_CHEF_CALIBRATE_START = 0x1000,
        S2E_CHEF_CALIBRATE_END = 0x1001,
        S2E_CHEF_CALIBRATE_CHECKPOINT = 0x1002
    };

    switch (op) {
    case 0:
        s2e_system_call(S2E_CHEF_CALIBRATE_START, NULL, 0);
        return 0;
    case 1:
        s2e_system_call(S2E_CHEF_CALIBRATE_CHECKPOINT, NULL, increment);
        return 0;
    case 2:
        s2e_system_call(S2E_CHEF_CALIBRATE_END, NULL, 0);
        return 0;
    default:
        return luaL_error(L, "invalid calibration operation");
    }
}


static const luaL_Reg symbexlib[] = {
	{ "symstring", symbex_symstring },
	{ "symint", symbex_symint },
	{ "killstate", symbex_killstate },
	{ "issymbolic", symbex_issymbolic },
	{ "calibrate", symbex_calibrate },
	{ NULL, NULL }
};


LUAMOD_API int luaopen_symbex(lua_State *L) {
	luaL_newlib(L, symbexlib);
	return 1;
}

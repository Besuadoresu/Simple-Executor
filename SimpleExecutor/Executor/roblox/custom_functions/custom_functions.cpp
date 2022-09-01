#include "custom_functions.hpp"
#include "../checks/checks.hpp"
#include "../../luau/lualib.h"

int getreg(lua_State* state)
{
    lua_pushvalue(state, LUA_REGISTRYINDEX);
    return 1;
}

int getgenv(lua_State* state)
{
    lua_pushvalue(state, LUA_GLOBALSINDEX);
    return 1;
}

int getrenv(lua_State* state)
{
	global_State* globalstate = state->global;

	lua_pushvalue((lua_State*)globalstate, LUA_GLOBALSINDEX);

	lua_xmove((lua_State*)globalstate, state, 1);

	return 1;
}

int getrawmetatable(lua_State* state)
{
	if (lua_gettop(state) < 1)
		luaL_error(state, "argument expected got nil");

	int type = lua_type(state, 1);

	if (type == LUA_TTABLE || type == LUA_TUSERDATA)
		return lua_getmetatable(state, 1) != 0;

	luaL_error(state, "Table or Userdata expected");

	return 0;
}

int getproperties(lua_State* state)
{
	if (lua_gettop(state) < 1)
		luaL_error(state, "1 argument expected");

	if (lua_type(state, 1) == LUA_TUSERDATA)
	{
		const auto instance = *reinterpret_cast<std::uintptr_t*>(lua_touserdata(state, 1));
		const auto class_descriptor = *reinterpret_cast<std::uintptr_t*>(instance + 0xC);

		lua_createtable(state, 0, 0);

		const auto start = *reinterpret_cast<std::uintptr_t*>(class_descriptor + 0x24);
		const auto end = *reinterpret_cast<std::uintptr_t*>(class_descriptor + 0x28);

		int iteration = 0u;

		for (auto i = start; i < end; i += 8)
		{
			const char* prop_name = *reinterpret_cast<const char**>(i);

			if (prop_name != nullptr && strlen(prop_name) > 0)
			{
				lua_pushinteger(state, ++iteration);
				lua_pushstring(state, prop_name);
				lua_settable(state, -3);
			}
		}

		return 1;
	}

	return 0;
}

void lua_register(lua_State* state, const char* name, lua_CFunction func)
{
	lua_pushcclosure(state, func, nullptr, 1);
	lua_setglobal(state, name);
}

void executor::roblox::custom_functions::push_functions(lua_State* state)
{
	lua_register(state, "getreg", getreg);
	lua_register(state, "getgenv", getgenv);
	lua_register(state, "getrenv", getgenv);
	lua_register(state, "getrawmetatable", getrawmetatable);
	lua_register(state, "getproperties", getproperties);
}
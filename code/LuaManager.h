// Handles Lua state getting/setting (LuaManager.h)

#pragma once

#include "lua.hpp"
#include "LuaBridge.h"

class LuaManager {

public:

	// This game's Lua state
	static inline lua_State *lua_state = nullptr;

	// Initialize the Lua state
	static void initialize() {

		// Create and assign new lua_State
		lua_state = luaL_newstate();

		// Open Lua libraries
		luaL_openlibs(lua_state);

	}

	// Get the Lua state
	static lua_State *get_lua_state() {
		return lua_state;
	}

};

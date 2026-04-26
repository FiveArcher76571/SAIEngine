// ComponentManager class declaration (ComponentManager.h)

#pragma once

#include <string>
#include <unordered_set>
#include <memory>
#include <filesystem>
#include <iostream>
#include "EngineTools.h"
#include "LuaManager.h"

#include "lua.hpp"
#include "LuaBridge.h"

class ComponentManager {

	// Container of LuaRef components
	static inline std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> component_list;

	// Number of times AddComponent has been called
	static inline int add_component_calls = 0;

public:

	// Find given component and create/return its LuaRef
	static std::shared_ptr<luabridge::LuaRef> get_component(const std::string &component) {

		// First check if we've already found it, and return true if so
		if (component_list.find(component) != component_list.end()) return component_list[component];

		// Add file extension to the component name
		std::string comp_file = "resources/component_types/" + component + ".lua";

		// Look in the filesystem for the component_types folder and for the given component
		if (!std::filesystem::exists("resources/component_types/") || !std::filesystem::exists(comp_file.c_str())) {

			// Throw an error if neither exist
			std::cout << "error: failed to locate component " << component;
			exit(0);

		}

		// Create a LuaRef for this component...

		// Check for errors in the Lua file while doing so
		if (luaL_dofile(LuaManager::get_lua_state(), comp_file.c_str()) != LUA_OK) {

			std::cout << "problem with lua file " << component;
			exit(0);

		}

		// Create the shared pointer
		std::shared_ptr<luabridge::LuaRef> new_ref = std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(LuaManager::get_lua_state(), component.c_str()));

		// Store it and return
		component_list.insert({ component, new_ref });
		return new_ref;

	}

	// Establish inheritance between tables
	static void establish_inheritance(luabridge::LuaRef &child, luabridge::LuaRef &parent) {

		// Taken from spec...

		// Create a Lua metatable
		luabridge::LuaRef new_metatable = luabridge::newTable(LuaManager::get_lua_state());

		// Set the "__index" field of this metatable to the parent
		new_metatable["__index"] = parent;

		// Set the child table as an instance
		// This uses Lua's raw C API (lua stack)...
		child.push(LuaManager::get_lua_state());
		new_metatable.push(LuaManager::get_lua_state());
		lua_setmetatable(LuaManager::get_lua_state(), -2);
		lua_pop(LuaManager::get_lua_state(), 1);

	}

	// Get and then increment add_component_calls
	static int get_add_component_calls() {

		return add_component_calls++;

	}

	// Flush loaded component list (e.g. on scene change)
	static void flush_loaded_comps() {

		component_list.clear();

	}

};


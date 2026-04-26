// Actor data type file (Actor.h)
// Contains the Actor type and related functions

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <filesystem>
#include <iostream>
#include "ComponentManager.h"
#include "EngineTools.h"
#include "LuaManager.h"

#include "rapidjson/document.h"
#include "glm/glm.hpp"
#include "SDL3/SDL.h"
#include "lua.hpp"
#include "LuaBridge.h"

// Holds a component's type and LuaRef
struct Component {

	// Component type
	std::string type = "";

	// Component LuaRef (unique to actor)
	std::shared_ptr<luabridge::LuaRef> ref = nullptr;

};

class Actor {

	// Helper: check if two colliders/triggers are intersecting return true if so
	// Uses AABB checking
	bool is_intersecting(const SDL_FRect &box1, const SDL_FRect &box2) {

		// if box1.left < box2.right && box1.right > box2.left && box1.top < box2.bottom && box1.bottom > box1.top
		// ...then it's colliding (taken from discussion)
		return box2.x < box1.x + box1.w &&
			box2.x + box2.w > box1.x &&
			box2.y < box1.y + box1.h &&
			box2.y + box2.h > box1.y;

	}

	// Helper: insertion sorter for component processing list
	void insert_alphabetical(const std::string &new_comp) {

		// Go through the list and find the element with the next letter (higher in ASCII)
		for (size_t i = 0; i < alphabetical_components.size(); i++) {
			if (alphabetical_components[i] > new_comp) {
				alphabetical_components.emplace(alphabetical_components.begin() + i, new_comp);
				return;
			}
		}

		// If we're here, just emplace back
		alphabetical_components.emplace_back(new_comp);

	}

	// Helper: insert a reference to this Actor object into a given component
	void insert_convenience_reference(std::shared_ptr<luabridge::LuaRef> component_ref) {

		// Taken from spec
		(*component_ref)["actor"] = this;

	}

	// List of components to be added at the end of the frame
	std::unordered_map<std::string, Component> components_to_add;

	// List of components to be removed at the end of the frame
	std::unordered_set<std::string> components_to_remove;

public:

	// Actor ID
	int id = -1;

	// Actor name
	std::string name = "";

	// List of components tied to actor
	// Key = component key for actor
	std::unordered_map<std::string, Component> components;

	// List of component names in order of processing (alphabetical)
	std::vector<std::string> alphabetical_components;

	//////
	////// Functions
	//////

	// Initialize Actor data given info from a JSON array/location
	void initialize(const rapidjson::Value &actor_data, const rapidjson::SizeType &i, const int &actor_id_counter);

	// Initialize Actor data given info from a JSON document
	void initialize(const rapidjson::Document &doc);

	// Initialize Actor data given another Actor
	void initialize(const Actor &to_copy);

	// Inject this actor's reference into all of its components
	// To be done once actors are stored in a single spot in memory
	void inject_references();

	// Call a given lifecycle function
	void call_lifecycle_function(const std::string &function_name);

	// Process components that need to be added
	void process_add_components() {

		// Add pending
		for (auto &comp : components_to_add) {

			// Add to main components list and alphabetical sorted list
			components.insert({ comp.first, comp.second });
			insert_alphabetical(comp.first);

		}

		// Start pending
		for (auto &comp : components_to_add) {

			// Call its OnStart lifecycle function
			(*comp.second.ref)["OnStart"](*comp.second.ref);

		}

		// Clear the list
		components_to_add.clear();

	}

	// Process components that need to be removed
	void process_remove_components() {

		// Remove pending
		for (const std::string &comp : components_to_remove) {

			// Remove from the main components list
			components.erase(comp);

			// Find and remove from alphabetical sorted list
			for (int i = 0; i < alphabetical_components.size(); i++) {

				if (comp == alphabetical_components[i]) {
					alphabetical_components.erase(alphabetical_components.begin() + i);
					break;
				}

			}

		}

		// Clear the remove list
		components_to_remove.clear();

	}

	// Toggle enable on all components
	void set_enable_all(const bool &enable) {

		for (const std::string &comp_name : alphabetical_components) {

			(*components[comp_name].ref)["enabled"] = enable;

		}

	}

	// Flush all components (before scene switch/actor deletion)
	void flush_components();

	/////
	///// Lua-exposed functions
	/////
	
	// Get this actor's name
	std::string GetName() {

		return name;

	}

	// Get this actor's ID
	int GetID() {

		return id;

	}

	// Get component by key name
	luabridge::LuaRef GetComponentByKey(const std::string &key) {

		return components.find(key) != components.end() && components_to_remove.find(key) == components_to_remove.end() ? *components[key].ref : luabridge::LuaRef(LuaManager::get_lua_state());

	}

	// Get first component of a given type
	luabridge::LuaRef GetComponent(const std::string &type) {

		// Go through the alphabetically sorted key list...
		for (std::string &comp_key : alphabetical_components) {

			// Skip if it's been removed
			if (components_to_remove.find(comp_key) != components_to_remove.end()) continue;

			// Return the first occurrence of the matching component type
			if (components[comp_key].type == type) return *components[comp_key].ref;

		}

		// If we haven't found anything, return nil
		return luabridge::LuaRef(LuaManager::get_lua_state());

	}

	// Get list of components of a given type
	luabridge::LuaRef GetComponents(const std::string &type) {

		// Make an empty table
		luabridge::LuaRef new_table = luabridge::newTable(LuaManager::get_lua_state());

		// Go through the alphabetically sorted key list...
		for (std::string &comp_key : alphabetical_components) {

			// Skip if it's been removed
			if (components_to_remove.find(comp_key) != components_to_remove.end()) continue;

			// Add every occurrence to the table
			if (components[comp_key].type == type) new_table[comp_key] = components[comp_key].ref;

		}

		return new_table;

	}

	// Add a component of given type to this actor
	luabridge::LuaRef AddComponent(const std::string &type) {

		// Pull base from the ComponentManager (required)
		std::shared_ptr<luabridge::LuaRef> comp_base = ComponentManager::get_component(type);

		// Make an instance of the component
		std::shared_ptr<luabridge::LuaRef> comp_instance = std::make_shared<luabridge::LuaRef>(luabridge::newTable(LuaManager::get_lua_state()));

		// Establish the new instance's inheritance
		ComponentManager::establish_inheritance(*comp_instance, *comp_base);

		// Get the actual ref from the shared pointer
		luabridge::LuaRef &comp_ref = *comp_instance;

		// The key is "r<n>" where n is the number of AddComponent calls thus far (stored in ComponentManager)
		std::string new_key = "r" + std::to_string(ComponentManager::get_add_component_calls());
		comp_ref["key"] = new_key;

		// Add enabled flag to the instance
		comp_ref["enabled"] = true;

		// Store the finished component to our pending-add list
		components_to_add.insert({ new_key,{ type, comp_instance } });

		// Return the finished reference
		return comp_ref;

	}

	// Remove a given component from this actor
	void RemoveComponent(const luabridge::LuaRef &to_remove) {

		// Find the component in the list...
		for (auto &comp : components) {

			// Once found...
			if (*comp.second.ref == to_remove) {

				// Disable the component
				(*comp.second.ref)["enabled"] = false;

				// Add it to the list of components to remove
				components_to_remove.insert(comp.first);

				// Done!
				return;

			}

		}

	}

};
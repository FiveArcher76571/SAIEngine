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
	void process_add_components();

	// Process components that need to be removed
	void process_remove_components();

	// Toggle enable on all components
	void set_enable_all(const bool &enable);

	// Flush all components (before scene switch/actor deletion)
	void flush_components();

	/////
	///// Lua-exposed functions
	/////
	
	// Get this actor's name
	std::string GetName();

	// Get this actor's ID
	int GetID();

	// Get component by key name
	luabridge::LuaRef GetComponentByKey(const std::string &key);

	// Get first component of a given type
	luabridge::LuaRef GetComponent(const std::string &type);

	// Get list of components of a given type
	luabridge::LuaRef GetComponents(const std::string &type);

	// Add a component of given type to this actor
	luabridge::LuaRef AddComponent(const std::string &type);

	// Remove a given component from this actor
	void RemoveComponent(const luabridge::LuaRef &to_remove);

};
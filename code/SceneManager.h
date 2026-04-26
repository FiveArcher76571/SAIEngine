// Scene manager header file (SceneManager.h)
// Holds SceneManager class declaration

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>
#include "Actor.h"
#include "GameSetup.h"
#include "ComponentManager.h"
#include "TemplateManager.h"
#include "Renderer.h"
#include "JSONReader.h"
#include "LuaManager.h"

#include "rapidjson/document.h"
#include "SDL3/SDL.h"
#include "lua.hpp"
#include "LuaBridge.h"

class SceneManager {

	// List of actors in order of scene definition
	static inline std::vector<std::shared_ptr<Actor>> actor_list;

	// Actors to be added at the beginning of each following frame
	static inline std::unordered_set<std::shared_ptr<Actor>> actors_to_add;

	// Actors to be removed at the end of each frame
	static inline std::unordered_set<std::shared_ptr<Actor>> actors_to_remove;

	// Template manager to store actor templates for this game
	static inline TemplateManager template_manager;

	// Actor ID counter
	static inline int actor_id_counter = 0;

	// Current scene name
	static inline std::string current_scene = "";

	// Scene to switch to (no switch if blank)
	static inline std::string new_scene = "";

public:

	// Initialize SceneManager using info from this game's renderer and game.config
	void initialize(Renderer &renderer, GameSetup &game_config);

	//////
	////// Actor-related functions
	//////

	// Goes through actor list and moves actors according to their parameters
	void update_actors();

	// Get the full list of actors
	std::vector<std::shared_ptr<Actor>> *get_actor_list();

	// Process actors to add
	void process_pending_add_actors();

	// Process actors to remove
	void process_pending_remove_actors();

	// Get an actor given its name
	static Actor *FindActor(const std::string &name) {

		// Go through list and return the first one with a matching name
		for (std::shared_ptr<Actor> &actor : actor_list) {

			if (actor->name == name && actors_to_remove.find(actor) == actors_to_remove.end()) return &(*actor);

		}

		// Return nil if it doesn't exist
		return nullptr;

	}

	// Get all Actors with a given name
	static luabridge::LuaRef FindAllActors(const std::string &name) {

		// Make an empty table
		luabridge::LuaRef new_table = luabridge::newTable(LuaManager::get_lua_state());

		// Counter for table indices
		int i = 1;

		// For every actor, if its name matches, add it to the table
		for (std::shared_ptr<Actor> &actor : actor_list) {

			if (actor->name == name && actors_to_remove.find(actor) == actors_to_remove.end()) new_table[i] = *actor;
			i++;

		}

		// Return the table
		return new_table;

	}

	// Instantiate a new Actor into this scene
	static Actor *InstantiateActor(const std::string &template_name) {

		// Create a new blank actor
		Actor new_actor;

		// Assign the template to it...
		// If the template doesn't exist, throw an error and exit
		if (!template_manager.check_and_assign_template(template_name, new_actor)) {
			std::cout << "error: template " << template_name << " is missing";
			exit(0);
		}

		// Assign its ID
		new_actor.id = actor_id_counter++;

		// All done!

		// Make it a shared pointer
		std::shared_ptr<Actor> actor_ptr = std::make_shared<Actor>(new_actor);

		// Put in list of actors to add
		actors_to_add.insert(actor_ptr);

		// Return a reference to the new actor
		return &(*actor_ptr);

	}

	// Destroy a given actor (remove from scene)
	static void DestroyActor(const Actor &to_destroy) {

		// Find the actor
		for (std::shared_ptr<Actor> actor : actor_list) {

			// If we've found it, time to destroy
			if (actor->id == to_destroy.id) {

				// Call its OnDestroy lifecycle function
				actor->call_lifecycle_function("OnDestroy");

				// Disable all of its components
				actor->flush_components();

				// Put it in the to-remove list
				actors_to_remove.insert(actor);

				// Done!
				return;

			}

		}

	}

	//////
	////// Scene-related functions
	//////

	// Get the current scene name
	static std::string GetCurrentScene() {

		return current_scene;

	}

	// Trigger scene switch
	static void TriggerSceneSwitch(const std::string &scene_name) {

		// Go throug every actor in the list...
		for (std::shared_ptr<Actor> actor : actor_list) {

			// Flush its components
			actor->flush_components();

		}

		// Set the scene switch name
		new_scene = scene_name;

	}
	
	// Switch to the given scene and update camera if necessary
	static void switch_scene(const std::string &scene_name);

};


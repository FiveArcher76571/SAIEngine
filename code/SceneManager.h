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
	static void update_actors();

	// Get the full list of actors
	static std::vector<std::shared_ptr<Actor>> *get_actor_list();

	// Process actors to add
	static void process_pending_add_actors();

	// Process actors to remove
	static void process_pending_remove_actors();

	// Get an actor given its name
	static Actor *FindActor(const std::string &name);

	// Get all Actors with a given name
	static luabridge::LuaRef FindAllActors(const std::string &name);

	// Instantiate a new Actor into this scene
	static Actor *InstantiateActor(const std::string &template_name);

	// Destroy a given actor (remove from scene)
	static void DestroyActor(const Actor &to_destroy);

	//////
	////// Scene-related functions
	//////

	// Get the current scene name
	static std::string GetCurrentScene();

	// Trigger scene switch
	static void TriggerSceneSwitch(const std::string &scene_name);
	
	// Switch to the given scene and update camera if necessary
	static void switch_scene(const std::string &scene_name);

};


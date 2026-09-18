// SceneManager class definition file (SceneManager.cpp)
// Holds functions to manage various actor movement/status changes/etc.

#include "SceneManager.h"

// Initialize SceneManager using info from game.config
void SceneManager::initialize(GameSetup &game_config) {

	// Initialize the template manager
	template_manager.initialize();

	// Add Actor reference functions
	luabridge::getGlobalNamespace(LuaManager::get_lua_state())
		.beginClass<Actor>("Actor")
		.addFunction("GetName", &Actor::GetName)
		.addFunction("GetID", &Actor::GetID)
		.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
		.addFunction("GetComponent", &Actor::GetComponent)
		.addFunction("GetComponents", &Actor::GetComponents)
		.addFunction("AddComponent", &Actor::AddComponent)
		.addFunction("RemoveComponent", &Actor::RemoveComponent)
		.endClass()
		.beginNamespace("Actor")
		.addFunction("Find", &FindActor)
		.addFunction("FindAll", &FindAllActors)
		.addFunction("Instantiate", &InstantiateActor)
		.addFunction("Destroy", &DestroyActor)
		.endNamespace();

	// Initialize the scene with the given start scene
	switch_scene(game_config.get_initial_scene());

}

//////
////// Actor-related functions
//////

// Goes through actor list and moves actors according to their parameters
void SceneManager::update_actors() {

	// Multiple loops through actors...

	// Process any pending actors to be added
	process_pending_add_actors();

	// Process and start any pending components to be added
	for (std::shared_ptr<Actor> &actor : actor_list) actor->process_add_components();

	// Execute OnUpdate lifecycle functions
	for (std::shared_ptr<Actor> &actor : actor_list) {

		// If the scene has switched after this, don't do anymore
		if (!new_scene.empty()) break;

		actor->call_lifecycle_function("OnUpdate");

	}

	// Done with all updates, now process late updates...
	for (std::shared_ptr<Actor> &actor : actor_list) {

		// If the scene has switched after this, don't do anymore
		if (!new_scene.empty()) break;

		actor->call_lifecycle_function("OnLateUpdate");

	}

	// Now process any pending components to be removed
	for (std::shared_ptr<Actor> &actor : actor_list) actor->process_remove_components();

	// Finally, process and pending actors to be removed
	process_pending_remove_actors();

	// If we've switched scenes, finalize that now
	if (!new_scene.empty()) switch_scene(new_scene);

}

// Get the full list of actors
std::vector<std::shared_ptr<Actor>> *SceneManager::get_actor_list() {

	return &actor_list;

}

// Process actors to add
void SceneManager::process_pending_add_actors() {

	// Now update the actors
	for (std::shared_ptr<Actor> actor : actors_to_add) {

		// Inject references of the actor to its components
		actor->inject_references();

		// Enable all of its components
		actor->set_enable_all(true);

		// Call its OnStart lifecycle function
		actor->call_lifecycle_function("OnStart");

		// Add it to the big list
		actor_list.push_back(actor);

	}

	// Clear all actors to add
	actors_to_add.clear();

}

// Process actors to remove
void SceneManager::process_pending_remove_actors() {

	// Go through and remove...
	for (std::shared_ptr<Actor> actor : actors_to_remove) {

		// Find and remove it from the list...
		for (int i = 0; i < actor_list.size(); i++) {

			if (actor_list[i]->id == actor->id) {

				actor_list.erase(actor_list.begin() + i);

			}
			break;

		}

	}

	// Clear the list
	actors_to_remove.clear();

}

// Get an actor given its name
Actor *SceneManager::FindActor(const std::string &name) {

	// Go through list and return the first one with a matching name
	for (std::shared_ptr<Actor> &actor : actor_list) {

		if (actor->name == name && actors_to_remove.find(actor) == actors_to_remove.end()) return &(*actor);

	}

	// Return nil if it doesn't exist
	return nullptr;

}

// Get all Actors with a given name
luabridge::LuaRef SceneManager::FindAllActors(const std::string &name) {

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
Actor *SceneManager::InstantiateActor(const std::string &template_name) {

	// Create a new blank actor
	Actor new_actor;

	// Assign the template to it...
	// If the template doesn't exist, throw an error and exit
	if (!template_manager.check_and_assign_template(template_name, new_actor)) {
		std::cout << "Error: The following template is missing: " << template_name;
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
void SceneManager::DestroyActor(const Actor &to_destroy) {

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
std::string SceneManager::GetCurrentScene() {

	return current_scene;

}

// Trigger scene switch
void SceneManager::TriggerSceneSwitch(const std::string &scene_name) {

	// Go throug every actor in the list...
	for (std::shared_ptr<Actor> actor : actor_list) {

		// Flush its components
		actor->flush_components();

	}

	// Set the scene switch name
	new_scene = scene_name;

}

// Switch to the given scene and update camera if necessary
void SceneManager::switch_scene(const std::string &scene_name) {

	// Clear all actors
	actor_list.clear();

	// Flush all loaded components
	ComponentManager::flush_loaded_comps();

	// Create a rapidJSON document to read in scene data
	rapidjson::Document scene_data;

	// Read in scene from filesystem, and throw error if it doesn't exist...
	if (!JSONReader::read_json(("resources/scenes/" + scene_name + ".scene").c_str(), scene_data)) {

		std::cout << "Error: The following scene is missing: " << scene_name;
		exit(0);

	}

	// Get actors array from scene document
	const rapidjson::Value &actor_array = scene_data["actors"];

	// Add actors one by one into actor list...
	for (rapidjson::SizeType i = 0; i < actor_array.Size(); i++) {

		Actor new_actor;

		// Check if this actor is using a template and populate it from there if so...
		if (actor_array[i].HasMember("template")) {

			// Check if this template exists, and if not throw an error and exit
			if (!template_manager.check_and_assign_template(actor_array[i]["template"].GetString(), new_actor)) {
				std::cout << "Error: The following template is missing: " << actor_array[i]["template"].GetString();
				exit(0);
			}

			// Once template is applied, add any additional/overridden actor info...

		}

		// Initialize (additional) info for this actor
		new_actor.initialize(actor_array, i, actor_id_counter++);

		// All done! Put the actor into the list
		actor_list.push_back(std::make_shared<Actor>(new_actor));

	}

	// Once all the actors are read in, go through them all...
	for (std::shared_ptr<Actor> &actor : actor_list) {

		// First, inject the actor reference into its components
		actor->inject_references();

		// Then call its OnStart function
		actor->call_lifecycle_function("OnStart");

	}

	// Update scene name
	current_scene = scene_name;

	// Reset switch scene
	new_scene = "";

}

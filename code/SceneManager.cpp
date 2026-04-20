// SceneManager class definition file (SceneManager.cpp)
// Holds functions to manage various actor movement/status changes/etc.

#include "SceneManager.h"

// Initialize SceneManager using info from this game's renderer and game.config
void SceneManager::initialize(Renderer &renderer, GameSetup &game_config) {

	// Initialize the reference to the image manager
	image_data = renderer.get_image_manager();

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
	// Also pass in a vec2 for the camera position...

	glm::vec2 new_camera_pos = { 0.0f, 0.0f };
	switch_scene(game_config.get_initial_scene());

	// Update camera position
	renderer.set_camera_pos(new_camera_pos, false);

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
	for (std::shared_ptr<Actor> &actor : actor_list) actor->call_lifecycle_function("OnUpdate");

	// Done with all updates, now process late updates...
	for (std::shared_ptr<Actor> &actor : actor_list) actor->call_lifecycle_function("OnLateUpdate");

	// Now process any pending components to be removed
	for (std::shared_ptr<Actor> &actor : actor_list) actor->process_remove_components();

	// Finally, process and pending actors to be removed
	process_pending_remove_actors();

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

		// Call its OnDestroy lifecycle functions
		actor->call_lifecycle_function("OnDestroy");

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

//////
////// Scene-related functions
//////

// Switch to the given scene and update camera if necessary
void SceneManager::switch_scene(const std::string &scene_name) {

	// Clear all actors
	actor_list.clear();

	// Create a rapidJSON document to read in scene data
	rapidjson::Document scene_data;

	// Read in scene from filesystem, and throw error if it doesn't exist...
	if (!JSONReader::read_json(("resources/scenes/" + scene_name + ".scene").c_str(), scene_data)) {

		std::cout << "error: scene " << scene_name << " is missing";
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
				std::cout << "error: template " << actor_array[i]["template"].GetString() << " is missing";
				exit(0);
			}

			// Once template is applied, add any additional/overridden actor info...

		}

		// Initialize (additional) info for this actor
		new_actor.initialize(actor_array, i, actor_id_counter);

		// All done! Put the actor into the list
		actor_list.push_back(std::make_shared<Actor>(new_actor));

		// Increment id counter
		actor_id_counter++;

	}

	// Once all the actors are read in, go through them all...
	for (std::shared_ptr<Actor> &actor : actor_list) {

		// First, inject the actor reference into its components
		actor->inject_references();

		// Then call its OnStart function
		actor->call_lifecycle_function("OnStart");

	}

}

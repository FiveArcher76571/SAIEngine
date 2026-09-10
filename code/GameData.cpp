// Game data definition file (GameData.cpp)
// Holds core game logic/data structure(s)

#include "GameData.h"

// Initialize the engine
void GameData::initialize() {

	// Get the list of modules to enable from config data
	rapidjson::Value module_list;
	
	// If it doesn't exist or it's empty, enable everything...
	if (!config_data.load_array("modules", module_list) || module_list.GetArray().Empty()) {
		mod_enable_all();
		return;
	}

	// Enable the default modules
	mod_enable_defaults();

	// Make unordered set from the list of modules
	std::unordered_set<std::string> modules_to_enable;
	for (rapidjson::Value::ConstValueIterator mod = module_list.Begin(); mod != module_list.End(); mod++) modules_to_enable.insert(mod->GetString());

	// Go through and enable given modules...

	// Window init flag
	bool window_init_done = false;

	// Image (needs window)
	if (modules_to_enable.find("Image") != modules_to_enable.end()) {
		if (!window_init_done) mod_enable_window();
		mod_enable_image();
	}

	// Text (needs window)
	if (modules_to_enable.find("Text") != modules_to_enable.end()) {
		if (!window_init_done) mod_enable_window();
		mod_enable_text();
	}

	// Camera (needs window)
	if (modules_to_enable.find("Camera") != modules_to_enable.end()) {
		if (!window_init_done) mod_enable_window();
		mod_enable_text();
	}

	// Input
	if (modules_to_enable.find("Input") != modules_to_enable.end()) mod_enable_image();

	// Audio
	if (modules_to_enable.find("Audio") != modules_to_enable.end()) mod_enable_audio();

	// Done with init stuff!
	
}

// Gameplay update actions
void GameData::gameplay_update() {

	// Don't do anything if no actors have been defined
	if (scene_manager.get_actor_list()->empty()) return;

	// Update the actors in the scene (every frame)
	scene_manager.update_actors();

	// Done with updates!

}

// Switch the gameplay scene, taking care of cleanup
void GameData::switch_gameplay_scene(const std::string &scene_name) {

	// Switch the scene in the scene manager
	// This updates the new camera position
	scene_manager.switch_scene(scene_name);

	// Clear the render
	renderer.clear_render();

}

// Start gameplay loop from a graphical window
void GameData::start() {

	// Initialize the engine
	initialize();

	// Run loop while the game hasn't been quit...
	while (game_state != GameState::QUIT) {

		// Get a new SDL input event
		SDL_Event input;

		// Run the loop on the SDL event
		while (SDL_PollEvent(&input)) {

			// Update input for the beginning of frame
			input_manager.update_states_bof(input);

			// If we receive a quit event, quit
			if (input.type == SDL_EVENT_QUIT) game_state = GameState::QUIT;

		}

		// Do update actions
		gameplay_update();

		// Clear the render
		renderer.clear_render();

		// Process any pending requests in defined order
		renderer.copy_queued_images();
		renderer.copy_queued_UI();
		renderer.copy_queued_text();
		renderer.copy_queued_pixels();

		// End of frame...

		// Present the render
		renderer.present_render();

		// Update frame count
		EngineTools::UpdateFrame();

		// Update keys
		input_manager.update_states_eof();

		// Update pending audio changes
		audio_manager.update();

	}

	// All done!
	return;

}
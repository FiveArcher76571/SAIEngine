// Game data definition file (GameData.cpp)
// Holds core game logic/data structure(s)

#include "GameData.h"

// Initialize the engine
void GameData::initialize() {

	// Initialize the modules map
	modules["Image"] = false;
	modules["Text"] = false;
	modules["Camera"] = false;
	modules["Input"] = false;
	modules["Audio"] = false;

	// Get the list of modules to enable from config data
	rapidjson::Value module_list;
	
	// If it doesn't exist or it's empty, enable everything...
	if (!config_data.load_array("modules", module_list) || module_list.GetArray().Empty()) {
		mod_enable_all();
		return;
	}

	// Enable the default modules
	mod_enable_defaults();

	// Update modules map for the ones to be loaded
	for (rapidjson::Value::ConstValueIterator mod = module_list.Begin(); mod != module_list.End(); mod++) modules[mod->GetString()] = true;

	// If only defaults are requested, return
	if (modules.find("Default") != modules.end()) return;

	// Go through and enable given modules...

	// Window init flag
	bool window_init_done = false;

	// Image (needs window)
	if (modules.find("Image") != modules.end()) {
		if (!window_init_done) mod_enable_window();
		mod_enable_image();
	}

	// Text (needs window)
	if (modules.find("Text") != modules.end()) {
		if (!window_init_done) mod_enable_window();
		mod_enable_text();
	}

	// Camera (needs window)
	if (modules.find("Camera") != modules.end()) {
		if (!window_init_done) mod_enable_window();
		mod_enable_text();
	}

	// Input
	if (modules.find("Input") != modules.end()) mod_enable_image();

	// Audio
	if (modules.find("Audio") != modules.end()) mod_enable_audio();

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

// Start gameplay loop
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
			if (modules.at("Input")) input_manager.update_states_bof(input);

			// If we receive a quit event, quit
			if (input.type == SDL_EVENT_QUIT) game_state = GameState::QUIT;

		}

		// Do update actions
		gameplay_update();

		// Rendering stuff, if those modules are loaded
		if (modules.at("Image") || modules.at("Text") || modules.at("Camera")) {

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

		}

		// Update frame count
		EngineTools::UpdateFrame();

		// Update keys
		if (modules.at("Input")) input_manager.update_states_eof();

		// Update pending audio changes
		if (modules.at("Audio")) audio_manager.update();

	}

	// All done!
	return;

}
// Game data definition file (GameData.cpp)
// Holds core game logic/data structure(s)

#include "GameData.h"

// Initialize the engine
void GameData::initialize() {

	// Initialize SDL, throw an error if failed
	if (!SDL_Init(SDL_INIT_VIDEO)) {

		std::cout << "Error: SDL couldn't initialize video!\nError:\n" << SDL_GetError();
		exit(0);

	}

	// Initialize SDL Audio too
	if (!SDL_Init(SDL_INIT_AUDIO)) {

		std::cout << "Error: SDL couldn't initialize audio!\nError:\n" << SDL_GetError();
		exit(0);

	}

	// Initialize Lua and LuaBridge...
	LuaManager::initialize();

	// Add Application namespace and functions
	luabridge::getGlobalNamespace(LuaManager::get_lua_state())
		.beginNamespace("Application")
		.addFunction("Quit", &EngineTools::Quit)
		.addFunction("Sleep", &EngineTools::Sleep)
		.addFunction("OpenURL", &EngineTools::OpenURL)
		.addFunction("GetFrame", &EngineTools::GetFrameNumber)
		.endNamespace();

	// Add Debug logging functionality
	luabridge::getGlobalNamespace(LuaManager::get_lua_state())
		.beginNamespace("Debug")
		.addFunction("Log", &EngineTools::Log)
		.addFunction("LogError", &EngineTools::LogError)
		.endNamespace();

	// Add glm::vec2 class to Lua
	luabridge::getGlobalNamespace(LuaManager::get_lua_state())
		.beginClass<glm::vec2>("vec2")
		.addProperty("x", &glm::vec2::x)
		.addProperty("y", &glm::vec2::y)
		.endClass();

	// Add Input namespace and functions
	luabridge::getGlobalNamespace(LuaManager::get_lua_state())
		.beginNamespace("Input")
		.addFunction("GetKey", &InputManager::GetKey)
		.addFunction("GetKeyDown", &InputManager::GetKeyDown)
		.addFunction("GetKeyUp", &InputManager::GetKeyUp)
		.addFunction("GetMousePosition", &InputManager::get_mouse_pos)
		.addFunction("GetMouseButton", &InputManager::mouse_down)
		.addFunction("GetMouseButtonDown", &InputManager::mouse_pressed)
		.addFunction("GetMouseButtonUp", &InputManager::mouse_released)
		.addFunction("GetMouseScrollDelta", &InputManager::get_mouse_scroll_delta)
		.addFunction("HideCursor", &InputManager::hide_cursor)
		.addFunction("ShowCursor", &InputManager::show_cursor)
		.endNamespace();

	// Add Text namespace and functions
	luabridge::getGlobalNamespace(LuaManager::get_lua_state())
		.beginNamespace("Text")
		.addFunction("Draw", &TextManager::CreateTextRenderRequest)
		.endNamespace();

	// Add Audio namespaces and functions...

	// Rendered Audio
	luabridge::getGlobalNamespace(LuaManager::get_lua_state())
		.beginNamespace("RAudio")
		.addFunction("Play", &AudioManager::Play)
		.addFunction("Halt", &AudioManager::Halt)
		.addFunction("SetVolume", &AudioManager::SetVolume)
		.endNamespace();

	// Synthesized Audio
	luabridge::getGlobalNamespace(LuaManager::get_lua_state())
		.beginNamespace("SAudio")
		.addFunction("Enable", &AudioManager::MIDI_EnablePlayback)
		.addFunction("EnableMIDIController", &AudioManager::MIDI_EnableControllerSupport)
		.addFunction("LoadMIDI", &AudioManager::MIDI_LoadMIDI)
		.addFunction("LoadSF2", &AudioManager::MIDI_LoadSF2)
		.addFunction("Play", &AudioManager::MIDI_Play)
		.addFunction("Pause", &AudioManager::MIDI_Pause)
		.addFunction("Resume", &AudioManager::MIDI_Resume)
		.addFunction("SetReverb", &AudioManager::MIDI_SetReverb)
		.addFunction("SetGain", &AudioManager::MIDI_SetGain)
		.addFunction("SetChannelVolume", &AudioManager::MIDI_SetChannelVolume)
		.addFunction("SetLoopPoints", &AudioManager::MIDI_SetLoopPoints)
		.addFunction("ResetLoopPoints", &AudioManager::MIDI_ResetLoopPoints)
		.addFunction("SendKeyOn", &AudioManager::MIDI_SendKeyOn)
		.addFunction("SendKeyOff", &AudioManager::MIDI_SendKeyOff)
		.endNamespace();


	// Initialize the scene manager
	scene_manager.initialize(renderer, config_data);

	// Initialize the audio manager
	audio_manager.initialize();

	// Initialize the renderer with game.config
	renderer.initialize(config_data);

	// Initialize input manager
	input_manager.initialize();

	// Done with init stuff!
}

// Gameplay update and render actions
void GameData::gameplay_update_and_render() {

	// Don't do anything if no actors have been defined
	if (scene_manager.get_actor_list()->empty()) return;

	// Update the actors in the scene (every frame)
	scene_manager.update_actors();

	// Done with updates, now start rendering!

	// Clear the render
	renderer.clear_render();

}

// Switch the gameplay scene, taking care of cleanup
void GameData::switch_gameplay_scene(const std::string &scene_name) {

	// Get the new camera position
	glm::vec2 new_camera_pos = { 0.0f, 0.0f };

	// Switch the scene in the scene manager
	// This updates the new camera position
	scene_manager.switch_scene(scene_name);

	// Update the camera in the renderer
	renderer.set_camera_pos(new_camera_pos, false);

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

		// Do update and render actions
		gameplay_update_and_render();

		// Process any text render requests
		renderer.copy_queued_text();

		// At the end of the frame, present the render
		renderer.present_render();

		// End of frame...

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
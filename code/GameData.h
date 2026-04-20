// Game data header file (GameData.h)
// Holds GameData class declaration

#pragma once

#include <iostream>
#include <string>
#include "GameSetup.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "AudioManager.h"
#include "InputManager.h"
#include "ComponentManager.h"
#include "LuaManager.h"

#include "fluidsynth.h"
#include "glm/glm.hpp"
#include "SDL3/SDL.h"
#include "lua.hpp"
#include "LuaBridge.h"

// State tracker enum class
enum class GameState {
	GAMEPLAY,
	QUIT
};

// Game data structure
// Holds stats and various helpful gameplay functions
class GameData {

	// Data pulled from resources/game.config
	static inline GameSetup config_data;

	// SceneManager object to manage actors
	static inline SceneManager scene_manager;

	// This game's renderer (deals with SDL rendering)
	static inline Renderer renderer;

	// This game's audio manager
	static inline AudioManager audio_manager;

	// This game's input manager
	static inline InputManager input_manager;

	// Game state tracker
	static inline GameState game_state = GameState::GAMEPLAY;

public:

	// Initialize the engine
	void initialize();

	// Gameplay update and render actions
	void gameplay_update_and_render();

	// Switch the gameplay scene, taking care of cleanup
	void switch_gameplay_scene(const std::string &scene_name);

	// Start gameplay loop from a graphical window
	void start();

};
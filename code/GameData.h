// Game data header file (GameData.h)
// Holds GameData class declaration

#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
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

	/////
	///// Functions
	/////

	// Enable default modules
	static void mod_enable_defaults() {

		// Initialize Lua and LuaBridge...
		LuaManager::initialize();

		// Add glm::vec2 class to Lua
		luabridge::getGlobalNamespace(LuaManager::get_lua_state())
			.beginClass<glm::vec2>("vec2")
			.addProperty("x", &glm::vec2::x)
			.addProperty("y", &glm::vec2::y)
			.endClass();

		// Add glm::ivec2 class to Lua
		luabridge::getGlobalNamespace(LuaManager::get_lua_state())
			.beginClass<glm::ivec2>("ivec2")
			.addProperty("x", &glm::ivec2::x)
			.addProperty("y", &glm::ivec2::y)
			.endClass();

		// Add Application namespace and functions
		luabridge::getGlobalNamespace(LuaManager::get_lua_state())
			.beginNamespace("Application")
			.addFunction("Quit", &EngineTools::Quit)
			.addFunction("Sleep", &EngineTools::Sleep)
			.addFunction("OpenURL", &EngineTools::OpenURL)
			.addFunction("GetFrame", &EngineTools::GetFrameNumber)
			.endNamespace();

		// Add console logging functionality
		luabridge::getGlobalNamespace(LuaManager::get_lua_state())
			.beginNamespace("Console")
			.addFunction("Print", &EngineTools::Print)
			.addFunction("PrintError", &EngineTools::PrintError)
			.endNamespace();

		// Scene switching
		luabridge::getGlobalNamespace(LuaManager::get_lua_state())
			.beginNamespace("Scene")
			.addFunction("Switch", &SceneManager::TriggerSceneSwitch)
			.addFunction("GetName", &SceneManager::GetCurrentScene)
			.endNamespace();

		// Initialize the scene manager
		scene_manager.initialize(config_data);

	}

	// Enable window module (+renderer)
	static void mod_enable_window() {

		// Initialize SDL, throw an error if failed
		if (!SDL_Init(SDL_INIT_VIDEO)) {

			std::cout << "Error: SDL couldn't initialize video!\n" << SDL_GetError();
			exit(0);

		}

		// Initialize the renderer with game.config
		renderer.initialize(config_data);

		// Window details
		luabridge::getGlobalNamespace(LuaManager::get_lua_state())
			.beginNamespace("Window")
			.addFunction("GetSize", &Renderer::GetWindowSize)
			.addFunction("SetSize", &Renderer::SetWindowSize)
			.addFunction("GetPos", &Renderer::GetWindowPos)
			.addFunction("SetPos", &Renderer::SetWindowPos)
			.endNamespace();

	}

	// Enable images module
	static void mod_enable_image() {

		// Image draws
		luabridge::getGlobalNamespace(LuaManager::get_lua_state())
			.beginNamespace("Image")
			.addFunction("DrawUI", &Renderer::DrawUI)
			.addFunction("DrawUIEx", &Renderer::DrawUIEx)
			.addFunction("Draw", &Renderer::Draw)
			.addFunction("DrawEx", &Renderer::DrawEx)
			.addFunction("DrawPixel", &Renderer::DrawPixel)
			.endNamespace();

	}

	// Enable text module
	static void mod_enable_text() {

		// Add Text namespace and functions
		luabridge::getGlobalNamespace(LuaManager::get_lua_state())
			.beginNamespace("Text")
			.addFunction("Draw", &TextManager::CreateTextRenderRequest)
			.endNamespace();

	}

	// Enable camera module
	static void mod_enable_camera() {

		// Camera functions
		luabridge::getGlobalNamespace(LuaManager::get_lua_state())
			.beginNamespace("Camera")
			.addFunction("SetPosition", &Renderer::SetCameraPos)
			.addFunction("GetPosition", &Renderer::GetCameraPos)
			.addFunction("SetZoom", &Renderer::SetZoom)
			.addFunction("GetZoom", &Renderer::GetZoomFactor)
			.endNamespace();

	}

	// Enable input module
	static void mod_enable_input() {

		// Add Input namespace and functions
		luabridge::getGlobalNamespace(LuaManager::get_lua_state())
			.beginNamespace("Input")
			.addFunction("EnableMIDIControl", &SequencedAudio::EnableRoutingAsInput)
			.addFunction("GetKey", &InputManager::GetKey)
			.addFunction("GetKeyDown", &InputManager::GetKeyDown)
			.addFunction("GetKeyUp", &InputManager::GetKeyUp)
			.addFunction("GetMIDI", &InputManager::GetMIDI)
			.addFunction("GetMIDIDown", &InputManager::GetMIDIDown)
			.addFunction("GetMIDIUp", &InputManager::GetMIDIUp)
			.addFunction("GetMIDIVelocity", &InputManager::GetMIDIKeyVelocity)
			.addFunction("GetMousePosition", &InputManager::get_mouse_pos)
			.addFunction("GetMouseButton", &InputManager::mouse_down)
			.addFunction("GetMouseButtonDown", &InputManager::mouse_pressed)
			.addFunction("GetMouseButtonUp", &InputManager::mouse_released)
			.addFunction("GetMouseScroll", &InputManager::get_mouse_scroll_delta)
			.addFunction("HideCursor", &InputManager::hide_cursor)
			.addFunction("ShowCursor", &InputManager::show_cursor)
			.endNamespace();

		// Initialize input manager
		input_manager.initialize();

	}

	// Enable audio module
	static void mod_enable_audio() {

		// Initialize SDL Audio too
		if (!SDL_Init(SDL_INIT_AUDIO)) {

			std::cout << "Error: SDL couldn't initialize audio!\n" << SDL_GetError();
			exit(0);

		}

		// Initialize the audio manager
		audio_manager.initialize();

	}

	// Enable all modules
	static void mod_enable_all() {

		// Go through and enable all by calling the functions
		mod_enable_defaults();
		mod_enable_window();
		mod_enable_image();
		mod_enable_text();
		mod_enable_camera();
		mod_enable_input();
		mod_enable_audio();

	}


public:

	// Initialize the engine
	void initialize();

	// Gameplay update actions
	void gameplay_update();

	// Switch the gameplay scene, taking care of cleanup
	void switch_gameplay_scene(const std::string &scene_name);

	// Start gameplay loop from a graphical window
	void start();

};
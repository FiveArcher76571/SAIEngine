// Input manager header file (InputManager.h)
// Holds InputManager class declaration

#pragma once

#include <vector>
#include <unordered_map>

#include "SDL3/SDL.h"
#include "glm/glm.hpp"
#include "fluidsynth.h"
#include "keycode_to_scancode.h"

enum class InputState {

	KEY_UP,
	KEY_JUST_DOWN,
	KEY_DOWN,
	KEY_JUST_UP

};

class InputManager {

	// Map of all key codes and their current states
	static inline std::unordered_map<SDL_Scancode, InputState> key_states;

	// List of keys that were pressed down this frame
	static inline std::vector<SDL_Scancode> keys_down_this_frame;

	// List of keys that were released this frame
	static inline std::vector<SDL_Scancode> keys_up_this_frame;

	// Current mouse position
	static inline glm::vec2 mouse_pos = glm::vec2(0);

	// Amount of mouse scroll this frame
	static inline float mouse_scroll_delta = 0.0f;

	// Map of all mouse button codes and their current states
	static inline std::unordered_map<int, InputState> mouse_states;

	// List of mouse buttons that were pressed down this frame
	static inline std::vector<int> mouse_down_this_frame;

	// List of mouse buttons that were released this frame
	static inline std::vector<int> mouse_up_this_frame;

	// MIDI button list [0, 127] and their current states
	static inline std::unordered_map<int, InputState> midi_states;

	// MIDI button current velocity tracker
	static inline std::unordered_map<int, int> midi_velocities;

	// List of MIDI keys that were pressed down this frame
	static inline std::vector<int> midi_down_this_frame;

	// List of MIDI keys that were released this frame
	static inline std::vector<int> midi_up_this_frame;

public:

	// Initialize the key states to default (up)
	static void initialize();

	// Update key states at the beginning of frame
	static void update_states_bof(const SDL_Event &input);

	// Callback function to update MIDI controller states
	static int update_states_midi(void *data, fluid_midi_event_t *event) {

		// Get the event's type
		int type = fluid_midi_event_get_type(event);

		// If the note has been pressed, track that
		// Note on = [144,159]
		if (type >= 144 && type <= 159) {

			// Get the key number pressed
			int key = fluid_midi_event_get_key(event);

			// Get the key's velocity
			int velocity = fluid_midi_event_get_velocity(event);

			// Update velocity in map
			midi_velocities[key] = fluid_midi_event_get_velocity(event);

			// If the velocity is 0, the note has been released, so assign the key state accordingly
			if (velocity == 0) {

				// Update state in map
				midi_states[key] = InputState::KEY_JUST_UP;

				// Update processing queue
				midi_up_this_frame.push_back(key);

			}

			else {

				// Update state in map
				midi_states[key] = InputState::KEY_JUST_DOWN;

				// Update processing queue
				midi_down_this_frame.push_back(key);

			}

		}

		// Return the value of the original callback function
		return fluid_midi_router_handle_midi_event(data, event);

	}

	// Update key states at the end of frame
	static void update_states_eof();

	/////
	///// Info fetchers
	/////
	
	// Checks if the given key is down
	static bool key_down(const SDL_Scancode &scancode);

	// Checks if the given key was pressed this frame
	static bool key_pressed(const SDL_Scancode &scancode);

	// Checks if the given key was released this frame
	static bool key_released(const SDL_Scancode &scancode);

	// Same as above three, but takes in keycodes, as given in keycode_to_scancode.h
	static bool GetKey(const std::string &keycode);
	static bool GetKeyDown(const std::string &keycode);
	static bool GetKeyUp(const std::string &keycode);

	// MIDI key checkers
	static bool GetMIDI(const int &key);
	static bool GetMIDIDown(const int &key);
	static bool GetMIDIUp(const int &key);

	// Checks if the given mouse button is down
	static bool mouse_down(const int &mouse_button);

	// Checks if the given mouse button was pressed this frame
	static bool mouse_pressed(const int &mouse_button);

	// Checks if the given mouse button was released this frame
	static bool mouse_released(const int &mouse_button);

	// Get the current mouse position
	static glm::vec2 get_mouse_pos();

	// Get the amount of scroll on the mouse wheel
	static float get_mouse_scroll_delta();

	// Hide the cursor using an SDL call
	static void hide_cursor();

	// Show the cursor using an SDL call
	static void show_cursor();

};


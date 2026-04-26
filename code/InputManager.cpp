// InputManager class definition file (InputManager.cpp)
// Deals with SDL_Event management

#include "InputManager.h"

// Initialize the key states to default (up)
void InputManager::initialize() {

	// Go through all possible keys and add them to the key map
	for (int key_code = SDL_SCANCODE_UNKNOWN; key_code < SDL_SCANCODE_COUNT; key_code++) {

		key_states[static_cast<SDL_Scancode>(key_code)] = InputState::KEY_UP;

	}

	// Do the same for mouse buttons
	for (int mouse_button = SDL_BUTTON_LEFT; mouse_button < SDL_BUTTON_X1; mouse_button++) {

		mouse_states[mouse_button] = InputState::KEY_UP;

	}

	// Do the same for the MIDI keys
	for (int midi_key = 0; midi_key < 128; midi_key++) {

		midi_states[midi_key] = InputState::KEY_UP;

	}

}

// Update key states at the beginning of frame
void InputManager::update_states_bof(const SDL_Event &input) {

	// Update key state if it's down this frame
	if (input.type == SDL_EVENT_KEY_DOWN) {

		key_states[SDL_GetScancodeFromKey(input.key.key, NULL)] = InputState::KEY_JUST_DOWN;
		keys_down_this_frame.push_back(SDL_GetScancodeFromKey(input.key.key, NULL));

	}

	// Update key state if it's up this frame
	else if (input.type == SDL_EVENT_KEY_UP) {

		key_states[SDL_GetScancodeFromKey(input.key.key, NULL)] = InputState::KEY_JUST_UP;
		keys_up_this_frame.push_back(SDL_GetScancodeFromKey(input.key.key, NULL));

	}

	// Do the same for mouse...

	// Update key state if it's down this frame
	else if (input.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {

		mouse_states[input.button.button] = InputState::KEY_JUST_DOWN;
		mouse_down_this_frame.push_back(input.button.button);

	}

	// Update key state if it's up this frame
	else if (input.type == SDL_EVENT_MOUSE_BUTTON_UP) {

		mouse_states[input.button.button] = InputState::KEY_JUST_UP;
		mouse_up_this_frame.push_back(input.button.button);

	}

	// Update mouse position
	else if (input.type == SDL_EVENT_MOUSE_MOTION) {

		mouse_pos = glm::vec2(input.motion.x, input.motion.y);

	}

	// Update mouse scroll
	else if (input.type == SDL_EVENT_MOUSE_WHEEL) {

		mouse_scroll_delta += input.wheel.y;

	}

}

// Update key states at the end of frame
void InputManager::update_states_eof() {

	// Go through the keys down this frame and set their states to be down
	for (const SDL_Scancode &key_code : keys_down_this_frame) {

		key_states[key_code] = InputState::KEY_DOWN;

	}

	// Clear the list
	keys_down_this_frame.clear();

	// Do the same for the keys up list
	for (const SDL_Scancode &key_code : keys_up_this_frame) {

		key_states[key_code] = InputState::KEY_UP;

	}

	keys_up_this_frame.clear();

	// Do the same for mouse...

	// Mouse button down
	for (const int &mouse_button : mouse_down_this_frame) {

		mouse_states[mouse_button] = InputState::KEY_DOWN;

	}

	mouse_down_this_frame.clear();

	// Mouse button up
	for (const int &mouse_button : mouse_up_this_frame) {

		mouse_states[mouse_button] = InputState::KEY_UP;

	}

	mouse_up_this_frame.clear();

	// Reset scroll wheel delta
	mouse_scroll_delta = 0.0f;

}

/////
///// Info fetchers
/////

// Checks if the given key is down
bool InputManager::key_down(const SDL_Scancode &scancode) {

	return key_states.find(scancode) == key_states.end() ? false : key_states[scancode] == InputState::KEY_DOWN;

}

// Checks if the given key was pressed this frame
bool InputManager::key_pressed(const SDL_Scancode &scancode) {

	return key_states.find(scancode) == key_states.end() ? false : key_states[scancode] == InputState::KEY_JUST_DOWN;

}

// Checks if the given key was released this frame
bool InputManager::key_released(const SDL_Scancode &scancode) {

	return key_states.find(scancode) == key_states.end() ? false : key_states[scancode] == InputState::KEY_JUST_UP;

}

// Same as above three, but takes in keycodes, as given in keycode_to_scancode.h

bool InputManager::GetKey(const std::string &keycode) {

	return __keycode_to_scancode.find(keycode) == __keycode_to_scancode.end() ? false : key_down(__keycode_to_scancode.at(keycode)) || key_pressed(__keycode_to_scancode.at(keycode));

}
bool InputManager::GetKeyDown(const std::string &keycode) {

	return __keycode_to_scancode.find(keycode) == __keycode_to_scancode.end() ? false : key_pressed(__keycode_to_scancode.at(keycode));

}
bool InputManager::GetKeyUp(const std::string &keycode) {

	return __keycode_to_scancode.find(keycode) == __keycode_to_scancode.end() ? false : key_released(__keycode_to_scancode.at(keycode));

}

// Checks if the given mouse button is down
bool InputManager::mouse_down(const int &mouse_button) {

	return mouse_states.find(mouse_button) == mouse_states.end() ? false : mouse_states[mouse_button] == InputState::KEY_DOWN || mouse_states[mouse_button] == InputState::KEY_JUST_DOWN;

}

// Checks if the given mouse button was pressed this frame
bool InputManager::mouse_pressed(const int &mouse_button) {

	return mouse_states.find(mouse_button) == mouse_states.end() ? false : mouse_states[mouse_button] == InputState::KEY_JUST_DOWN;

}

// Checks if the given mouse button was released this frame
bool InputManager::mouse_released(const int &mouse_button) {

	return mouse_states.find(mouse_button) == mouse_states.end() ? false : mouse_states[mouse_button] == InputState::KEY_JUST_UP;

}

// Get the current mouse position
glm::vec2 InputManager::get_mouse_pos() {

	return mouse_pos;

}

// Get the amount of scroll on the mouse wheel
float InputManager::get_mouse_scroll_delta() {

	return mouse_scroll_delta;

}

// Hide the cursor using an SDL call
void InputManager::hide_cursor() {

	SDL_HideCursor();

}

// Show the cursor using an SDL call
void InputManager::show_cursor() {

	SDL_ShowCursor();

}
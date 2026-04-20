// Renderer class declaration file (Renderer.h)
// Holds declaration for Renderer class

#pragma once

#include <string>
#include <vector>
#include <queue>
#include "GameSetup.h"
#include "ImageManager.h"
#include "TextManager.h"
#include "Actor.h"
#include "JSONReader.h"

#include "rapidjson/document.h"
#include "SDL3/SDL.h"

class Renderer {

	// SDL_Window object
	static inline SDL_Window *window;

	// Window x position on screen
	static inline int WINDOW_POS_X = 100;

	// Window y position on screen
	static inline int WINDOW_POS_Y = 100;

	// Window width
	static inline int window_width = 640;

	// Window height
	static inline int window_height = 360;

	// Camera position in world
	static inline glm::vec2 camera_pos = { 0.0f, 0.0f };

	// Camera position offset (in world units)
	static inline glm::vec2 camera_offset = { 0.0f, 0.0f };

	// Camera zoom factor
	static inline float camera_zoom = 1.0f;

	// Camera ease factor
	static inline float camera_ease = 1.0f;

	// SDL_Renderer object
	SDL_Renderer *renderer;

	// Color to clear renderer with (as an SDL_Color type)
	static inline SDL_Color clear_color = { 255, 255, 255, 255 };

	// Stores image/texture data
	ImageManager image_data;

	// Stores text data
	TextManager text_data;

	///// Functions

	// Set a zoom factor for the upcoming rendering
	void set_zoom(const float &zoom_factor) {

		// Set the zoom factor for the renderer using SDL_RenderSetScale
		// If unsuccessful throw an error
		if (!SDL_SetRenderScale(renderer, zoom_factor, zoom_factor)) {

			std::cout << "Error: SDL couldn't set the zoom factor!\nError:\n" << SDL_GetError();
			exit(0);

		}

	}

public:

	// Renderer initializer
	// Gets game.config passed in and pulls other data from resources/rendering.config
	// Also initializes the ImageManager and TextManager (using game.config)
	void initialize(GameSetup &game_config);

	// Get a pointer to the image manager
	ImageManager *get_image_manager();

	// Get the window width
	int get_window_width();

	// Get the window height
	int get_window_height();

	// Get the current camera position (automatically apply offset)
	glm::vec2 get_camera_pos();

	// Set the camera position to the given vec2 (ignoring offset)
	// Does easing if flag is set
	void set_camera_pos(const glm::vec2 &new_pos, const bool &easing);

	// Clear the renderer with the default clear color
	void clear_render();

	// Clear the renderer with the given color object
	void clear_render(SDL_Color &color);

	// Copy queued text draw requests
	void copy_queued_text();

	// Present the render to the screen
	void present_render();

};


// Renderer class declaration file (Renderer.h)
// Holds declaration for Renderer class

#pragma once

#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include "GameSetup.h"
#include "ImageManager.h"
#include "TextManager.h"
#include "Actor.h"
#include "JSONReader.h"

#include "rapidjson/document.h"
#include "SDL3/SDL.h"

// Window-related variables
struct Window {

	// SDL_Window object
	static inline SDL_Window *SDL_Window;

	// Window position/dimensions on screen
	static inline SDL_Rect rect = { 100, 100, 640, 360 };

};

// Camera-related variables
struct Camera {

	// Camera position in world
	static inline glm::vec2 pos = { 0.0f, 0.0f };

	// Camera position offset (in world units)
	static inline glm::vec2 offset = { 0.0f, 0.0f };

	// Camera zoom factor
	static inline float zoom = 1.0f;

	// Camera ease factor
	static inline float ease = 1.0f;

};

class Renderer {

	// Window details
	static inline Window window;

	// Camera details
	static inline Camera camera;

	// Pixels per meter of world coordinates
	static inline int PPM = 100;

	// SDL_Renderer object
	static inline SDL_Renderer *renderer;

	// Color to clear renderer with (as an SDL_Color type)
	static inline SDL_Color clear_color = { 255, 255, 255, 255 };

	// Stores image/texture data
	static inline ImageManager image_data;

	// Stores text data
	static inline TextManager text_data;

	///// Request Queues

	// Request ID counter
	static inline int req_id_counter = 0;

	// UI queue (screen coordinates)
	static inline std::vector<ImageDrawRequest> UI_requests;

	// Image queue (world coordinates)
	static inline std::vector<ImageDrawRequest> image_requests;

	// Pixel queue (screen coordinates)
	static inline std::vector<ImageDrawRequest> pixel_requests;

	///// Functions

	// ImageDrawRequest Comparator
	static bool ImageDrawRequestComp(const ImageDrawRequest &a, const ImageDrawRequest &b) {

		// First compare render order
		if (a.sorting_order > b.sorting_order) return true;
		else if (a.sorting_order < b.sorting_order) return false;

		// Then compare ID
		else if (a.id < b.id) return true;

		return false;

	}

public:

	/////
	///// Image Draw Requests
	/////

	// Draw UI elements
	static void DrawUI(const std::string &image_name, const int &pos_x, const int &pos_y) {

		// Create a new image draw request
		ImageDrawRequest new_req;

		// Populate image pointer
		new_req.image = ImageManager::get_image(image_name, renderer);

		// Populate position
		new_req.pos = { static_cast<float>(pos_x), static_cast<float>(pos_y), 0, 0 };

		// Get width and height from texture
		SDL_GetTextureSize(new_req.image->texture, &new_req.pos.w, &new_req.pos.h);

		// Populate ID
		new_req.id = req_id_counter++;

		// Add the new request to the queue
		Renderer::UI_requests.push_back(new_req);

	}

	// Draw UI elements ~extended~
	static void DrawUIEx(const std::string &image_name, const int &pos_x, const int &pos_y, const int &color_r, const int &color_g, const int &color_b, const int &color_a, const int &sorting_order) {

		// Create a new image draw request
		ImageDrawRequest new_req;

		// Populate image pointer
		new_req.image = ImageManager::get_image(image_name, renderer);

		// Populate position
		new_req.pos = { static_cast<float>(pos_x), static_cast<float>(pos_y), 0, 0 };

		// Get width and height from texture
		SDL_GetTextureSize(new_req.image->texture, &new_req.pos.w, &new_req.pos.h);

		// Populate color
		new_req.color = { Uint8(color_r), Uint8(color_g), Uint8(color_b), Uint8(color_a) };

		// Populate sorting order
		new_req.sorting_order = sorting_order;

		// Populate ID
		new_req.id = req_id_counter++;

		// Add the new request to the queue
		UI_requests.push_back(new_req);

	}

	// Draw images in world coordinates
	static void Draw(const std::string &image_name, const float &pos_x, const float &pos_y) {

		// Create a new image draw request
		ImageDrawRequest new_req;

		// Populate image pointer
		new_req.image = ImageManager::get_image(image_name, renderer);

		// Populate position
		new_req.pos = { static_cast<float>(pos_x), static_cast<float>(pos_y), 0, 0 };

		// Get width and height from texture
		SDL_GetTextureSize(new_req.image->texture, &new_req.pos.w, &new_req.pos.h);

		// Set pivot point
		SDL_GetTextureSize(new_req.image->texture, &new_req.pivot.x, &new_req.pivot.y);

		// Divide by two to set to center (default)
		new_req.pivot.x *= 0.5f;
		new_req.pivot.y *= 0.5f;

		// Populate ID
		new_req.id = req_id_counter++;

		// Add the new request to the queue
		image_requests.push_back(new_req);

	}

	// Draw images ~extended~
	static void DrawEx(const std::string &image_name, const float &pos_x, const float &pos_y, const int &rotation, const float &scale_x, const float &scale_y, const float &pivot_x, const float &pivot_y, const int &color_r, const int &color_g, const int &color_b, const int &color_a, const int &sorting_order) {

		// Create a new image draw request
		ImageDrawRequest new_req;

		// Populate image pointer
		new_req.image = ImageManager::get_image(image_name, renderer);

		// Populate position
		new_req.pos = { static_cast<float>(pos_x), static_cast<float>(pos_y), 0, 0 };

		// Get width and height from texture
		SDL_GetTextureSize(new_req.image->texture, &new_req.pos.w, &new_req.pos.h);

		// Populate rotation
		new_req.rotation = rotation;

		// Populate scale
		new_req.scale = glm::vec2(scale_x, scale_y);

		// Populate pivot
		new_req.pivot = { pivot_x, pivot_y };

		// Populate color
		new_req.color = { Uint8(color_r), Uint8(color_g), Uint8(color_b), Uint8(color_a) };

		// Populate sorting order
		new_req.sorting_order = sorting_order;

		// Populate ID
		new_req.id = req_id_counter++;

		// Add the new request to the queue
		image_requests.push_back(new_req);

	}

	// Draw pixel (screen coordinates)
	static void DrawPixel(const int &pos_x, const int &pos_y, const int &color_r, const int &color_g, const int &color_b, const int &color_a) {

		// Create a new image draw request
		ImageDrawRequest new_req;

		// Populate position
		new_req.pos = { static_cast<float>(pos_x), static_cast<float>(pos_y), 0, 0 };

		// Populate color
		new_req.color = { Uint8(color_r), Uint8(color_g), Uint8(color_b), Uint8(color_a) };

		// Populate ID
		new_req.id = req_id_counter++;

		// Add the new request to the queue
		pixel_requests.push_back(new_req);

	}

	///// Functions

	// Renderer initializer
	// Gets game.config passed in and pulls other data from resources/rendering.config
	// Also initializes the ImageManager and TextManager (using game.config)
	void initialize(GameSetup &game_config);

	// Get a pointer to the SDL renderer
	static SDL_Renderer *get_renderer() {

		return renderer;

	}

	// Get the window width
	static int get_window_width();

	// Get the window height
	static int get_window_height();

	// Get the current camera position (automatically apply offset)
	static glm::vec2 get_camera_pos();

	// Set the camera position to the given vec2 (ignoring offset)
	// Does easing if flag is set
	static void set_camera_pos(const glm::vec2 &new_pos, const bool &easing);

	// Set camera position (Lua exposed)
	static void SetCameraPos(const float &x, const float &y) {

		set_camera_pos(glm::vec2(x, y), true);

	}

	// Set a zoom factor for the upcoming rendering
	static void set_zoom(const float &zoom_factor) {

		// Set the zoom factor for the renderer using SDL_RenderSetScale
		// If unsuccessful throw an error
		if (!SDL_SetRenderScale(renderer, zoom_factor, zoom_factor)) {

			std::cout << "Error: SDL couldn't set the zoom factor!\nError:\n" << SDL_GetError();
			exit(0);

		}

		// Also update camera struct
		camera.zoom = zoom_factor;

	}

	// Get the current zoom factor
	static float GetZoomFactor() {

		return camera.zoom;

	}

	// Clear the renderer with the default clear color
	void clear_render();

	// Clear the renderer with the given color object
	void clear_render(SDL_Color &color);

	// Copy image requests (world coordinates)
	void copy_queued_images();

	// Copy UI requests (screen coordinates)
	void copy_queued_UI();

	// Copy pixel draw requests (screen coordinates)
	void copy_queued_pixels();

	// Copy queued text draw requests
	void copy_queued_text();

	// Present the render to the screen
	void present_render();

};


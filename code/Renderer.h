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

	// SDL_Window object
	static inline SDL_Window *window;

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

	// Renderer initializer
	// Gets game.config passed in and pulls other data from resources/rendering.config
	// Also initializes the ImageManager and TextManager (using game.config)
	static void initialize(GameSetup &game_config);

	// Get a pointer to the SDL renderer
	static SDL_Renderer *get_renderer();

	/////
	///// Window
	/////

	// Get the window width
	static int GetWindowWidth();

	// Get the window height
	static int GetWindowHeight();

	/////
	///// Camera
	/////

	// Get the current camera position (automatically apply offset)
	static glm::vec2 GetCameraPos();

	// Set the camera position to the given position (ignoring offset)
	// Does easing if flag is set
	static void SetCameraPos(const float &x, const float &y, const bool &easing);

	// Set a zoom factor for the upcoming rendering
	static void SetZoom(const float &zoom_factor);

	// Get the current zoom factor
	static float GetZoomFactor();

	/////
	///// Render Management
	/////

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

	/////
	///// Image Draw Requests
	/////

	// Draw UI elements
	static void DrawUI(const std::string &image_name, const int &pos_x, const int &pos_y);

	// Draw UI elements ~extended~
	static void DrawUIEx(const std::string &image_name, const int &pos_x, const int &pos_y, const int &color_r, const int &color_g, const int &color_b, const int &color_a, const int &sorting_order);

	// Draw images in world coordinates
	static void Draw(const std::string &image_name, const float &pos_x, const float &pos_y);

	// Draw images ~extended~
	static void DrawEx(const std::string &image_name, const float &pos_x, const float &pos_y, const int &rotation, const float &scale_x, const float &scale_y, const float &pivot_x, const float &pivot_y, const int &color_r, const int &color_g, const int &color_b, const int &color_a, const int &sorting_order);

	// Draw pixel (screen coordinates)
	static void DrawPixel(const int &pos_x, const int &pos_y, const int &color_r, const int &color_g, const int &color_b, const int &color_a);

};


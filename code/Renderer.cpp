// Renderer class definition file (Renderer.cpp)
// Holds SDL rendering data and functions

#include "Renderer.h"

// Renderer initializer
// Gets game.config passed in and pulls other data from resources/rendering.config
// Also initializes the ImageManager and TextManager (using game.config)
void Renderer::initialize(GameSetup &game_config) {

	// Get rendering info from rendering.config...
	
	// Get rendering config document
	rapidjson::Document rendering_config;

	// Read in rendering.config if it exists
	// If not, keep default parameters
	if (JSONReader::read_json("resources/rendering.config", rendering_config)) {

		// Keep defaults if a resolution isn't defined...
		if (rendering_config.HasMember("x_resolution")) window_width = rendering_config["x_resolution"].GetInt();
		if (rendering_config.HasMember("y_resolution")) window_height =  rendering_config["y_resolution"].GetInt();

		// Also read in clear color
		if (rendering_config.HasMember("clear_color_r")) clear_color.r = rendering_config["clear_color_r"].GetInt();
		if (rendering_config.HasMember("clear_color_g")) clear_color.g = rendering_config["clear_color_g"].GetInt();
		if (rendering_config.HasMember("clear_color_b")) clear_color.b = rendering_config["clear_color_b"].GetInt();

		// Also read in any potential camera offset
		if (rendering_config.HasMember("cam_offset_x")) camera_offset.x = rendering_config["cam_offset_x"].GetFloat();
		if (rendering_config.HasMember("cam_offset_y")) camera_offset.y = rendering_config["cam_offset_y"].GetFloat();

		// Also read zoom
		if (rendering_config.HasMember("zoom_factor")) camera_zoom = rendering_config["zoom_factor"].GetFloat();

		// Also read camera ease
		if (rendering_config.HasMember("cam_ease_factor")) camera_ease = rendering_config["cam_ease_factor"].GetFloat();

	}

	// Use the above info to create a shown SDL window
	window = SDL_CreateWindow(game_config.get_string("game_title").c_str(), window_width, window_height, 0);

	// If window creation failed, throw an error
	if (window == nullptr) {

		std::cout << "Error: SDL couldn't create a window!\nError:\n" << SDL_GetError();
		exit(0);

	}

	// Now create an SDL renderer
	// Enable VSync
	renderer = SDL_CreateRenderer(window, "");
	SDL_SetRenderVSync(renderer, 1);

	// If renderer creation failed, throw an error
	if (renderer == nullptr) {

		std::cout << "Error: SDL couldn't create a renderer!\nError:\n" << SDL_GetError();
		exit(0);

	}

	// Initialize the ImageManager with the renderer
	image_data.initialize(renderer);

	// Initialize the text manager
	text_data.initialize();

}

// Get a pointer to the image manager
ImageManager *Renderer::get_image_manager() {

	return &image_data;

}

// Get the window width
int Renderer::get_window_width() {

	return window_width;

}

// Get the window height
int Renderer::get_window_height() {

	return window_height;

}

// Get the current camera position (automatically apply offset)
glm::vec2 Renderer::get_camera_pos() {

	return camera_pos + camera_offset;

}

// Set the camera position to the given vec2 (ignoring offset)
// Does easing if flag is set
void Renderer::set_camera_pos(const glm::vec2 &new_pos, const bool &easing) {

	// If we don't want easing, immediately set the camera position and move on
	if (!easing) {

		camera_pos = new_pos;
		return;

	}

	// Otherwise, mix the two positions using glm::mix and the defined easing factor
	camera_pos = glm::mix(camera_pos, new_pos, camera_ease);

}

// Clear the renderer with the default clear color
void Renderer::clear_render() {

	SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	SDL_RenderClear(renderer);

}

// Clear the renderer with the given color object
void Renderer::clear_render(SDL_Color &color) {

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(renderer);

}

// Copy queued text draw requests
void Renderer::copy_queued_text() {

	// Go through and start rendering the queued Text objects...
	while (!text_data.render_queue.empty()) {

		Text &curr_text = text_data.render_queue.front();

		// Temporary FRect
		SDL_FRect float_pos = { static_cast<float>(curr_text.position.x), static_cast<float>(curr_text.position.y), static_cast<float>(curr_text.position.w), static_cast<float>(curr_text.position.h) };

		SDL_RenderTexture(renderer, SDL_CreateTextureFromSurface(renderer, curr_text.surface), nullptr, &float_pos);

		// Pop from the queue
		text_data.render_queue.pop();

	}

}

// Present the render to the screen
void Renderer::present_render() {

	SDL_RenderPresent(renderer);

}

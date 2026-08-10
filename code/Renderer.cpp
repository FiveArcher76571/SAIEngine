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
		if (rendering_config.HasMember("x_resolution")) window.rect.w = rendering_config["x_resolution"].GetInt();
		if (rendering_config.HasMember("y_resolution")) window.rect.h =  rendering_config["y_resolution"].GetInt();

		// Also read in clear color
		if (rendering_config.HasMember("clear_color_r")) clear_color.r = rendering_config["clear_color_r"].GetInt();
		if (rendering_config.HasMember("clear_color_g")) clear_color.g = rendering_config["clear_color_g"].GetInt();
		if (rendering_config.HasMember("clear_color_b")) clear_color.b = rendering_config["clear_color_b"].GetInt();

		// Also read in any potential camera offset
		if (rendering_config.HasMember("cam_offset_x")) camera.offset.x = rendering_config["cam_offset_x"].GetFloat();
		if (rendering_config.HasMember("cam_offset_y")) camera.offset.y = rendering_config["cam_offset_y"].GetFloat();

		// Also read zoom
		if (rendering_config.HasMember("zoom_factor")) camera.zoom = rendering_config["zoom_factor"].GetFloat();

		// Also read camera ease
		if (rendering_config.HasMember("cam_ease_factor")) camera.ease = rendering_config["cam_ease_factor"].GetFloat();

	}

	// Use the above info to create a shown SDL window
	window.SDL_Window = SDL_CreateWindow(game_config.get_string("game_title").c_str(), window.rect.w, window.rect.h, 0);

	// If window creation failed, throw an error
	if (window.SDL_Window == nullptr) {

		std::cout << "Error: SDL couldn't create a window!\nError:\n" << SDL_GetError();
		exit(0);

	}

	// Now create an SDL renderer
	// Enable VSync
	renderer = SDL_CreateRenderer(window.SDL_Window, "");
	SDL_SetRenderVSync(renderer, 1);

	// If renderer creation failed, throw an error
	if (renderer == nullptr) {

		std::cout << "Error: SDL couldn't create a renderer!\nError:\n" << SDL_GetError();
		exit(0);

	}

	// Initialize the text manager
	text_data.initialize();

}

// Get a pointer to the SDL renderer
SDL_Renderer *Renderer::get_renderer() {

	return renderer;

}

// Get the window width
int Renderer::get_window_width() {

	return window.rect.w;

}

// Get the window height
int Renderer::get_window_height() {

	return window.rect.h;

}

// Get the current camera position (automatically apply offset)
glm::vec2 Renderer::GetCameraPos() {

	return camera.pos + camera.offset;

}

// Set the camera position to the given position (ignoring offset)
// Does easing if flag is set
void Renderer::SetCameraPos(const float &x, const float &y, const bool &easing) {

	// Put the new position in a vec2
	glm::vec2 new_pos = glm::vec2(x, y);

	// If we don't want easing, immediately set the camera position and move on
	if (!easing) {

		camera.pos = new_pos;
		return;

	}

	// Otherwise, mix the two positions using glm::mix and the defined easing factor
	camera.pos = glm::mix(camera.pos, new_pos, camera.ease);

}

// Get the current zoom factor
float Renderer::GetZoomFactor() {

	return camera.zoom;

}

// Set a zoom factor for the upcoming rendering
void Renderer::SetZoom(const float &zoom_factor) {

	// Set the zoom factor for the renderer using SDL_RenderSetScale
	// If unsuccessful throw an error
	if (!SDL_SetRenderScale(renderer, zoom_factor, zoom_factor)) {

		std::cout << "Error: SDL couldn't set the zoom factor!\nError:\n" << SDL_GetError();
		exit(0);

	}

	// Also update camera struct
	camera.zoom = zoom_factor;

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

// Copy image requests (world coordinates)
void Renderer::copy_queued_images() {

	// Set zoom
	SetZoom(camera.zoom);

	// Stable sort the queue
	std::stable_sort(image_requests.begin(), image_requests.end(), ImageDrawRequestComp);

	// Go through and render all the images
	for (ImageDrawRequest &req : image_requests) {

		// Get final render position
		SDL_FRect final_render_pos = req.pos;

		// Translate based on camera position
		final_render_pos.x -= GetCameraPos().x;
		final_render_pos.y -= GetCameraPos().y;

		// Apply flipping...

		int flipping = SDL_FLIP_NONE;

		// Flip axis based on whether scale is negative on a given axis
		if (req.scale.x < 0) flipping |= SDL_FLIP_HORIZONTAL;
		if (req.scale.y < 0) flipping |= SDL_FLIP_VERTICAL;

		// Set scales positive
		glm::vec2 abs_scale = glm::abs(req.scale);

		// Scale the width and height
		final_render_pos.w *= abs_scale.x;
		final_render_pos.h *= abs_scale.y;

		// Scale pivot point
		int pivot_x = static_cast<int>(req.pivot.x * abs_scale.x);
		int pivot_y = static_cast<int>(req.pivot.y * abs_scale.y);
		SDL_FPoint scaled_pivot = { static_cast<float>(pivot_x), static_cast<float>(pivot_y) };

		// Translate final position based on all factors (given in spec)
		final_render_pos.x = static_cast<int>(final_render_pos.x * PPM + window.rect.w * 0.5f * (1.0f / camera.zoom) - scaled_pivot.x);
		final_render_pos.y = static_cast<int>(final_render_pos.y * PPM + window.rect.h * 0.5f * (1.0f / camera.zoom) - scaled_pivot.y);

		// Apply tint/alpha
		SDL_SetTextureColorMod(req.image->texture, req.color.r, req.color.g, req.color.b);
		SDL_SetTextureAlphaMod(req.image->texture, req.color.a);

		// Copy the final image to the buffer
		SDL_RenderTextureRotated(renderer, req.image->texture, NULL, &final_render_pos, req.rotation, &scaled_pivot, static_cast<SDL_FlipMode>(flipping));

		// Reset tint/alpha
		SDL_SetTextureColorMod(req.image->texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(req.image->texture, 255);

	}

	// Reset zoom
	SetZoom(1);

}

// Copy UI requests (screen coordinates)
void Renderer::copy_queued_UI() {

	// Stable sort the queue
	std::stable_sort(UI_requests.begin(), UI_requests.end(), ImageDrawRequestComp);

	// Go through and render all the UI images
	for (ImageDrawRequest &req : UI_requests) {

		// Apply tint/alpha
		SDL_SetTextureColorMod(req.image->texture, req.color.r, req.color.g, req.color.b);
		SDL_SetTextureAlphaMod(req.image->texture, req.color.a);

		// Copy the final image to the buffer
		SDL_RenderTextureRotated(renderer, req.image->texture, NULL, &req.pos, req.rotation, &req.pivot, SDL_FLIP_NONE);

		// Reset tint/alpha
		SDL_SetTextureColorMod(req.image->texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(req.image->texture, 255);

	}

}

// Copy pixel draw requests (screen coordinates)
void Renderer::copy_queued_pixels() {

	// Stable sort the queue
	std::stable_sort(pixel_requests.begin(), pixel_requests.end(), ImageDrawRequestComp);

	// Set blend mode
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// Go through and render all the UI images
	for (ImageDrawRequest &req : pixel_requests) {

		// Apply color/alpha
		SDL_SetRenderDrawColor(renderer, req.color.r, req.color.g, req.color.b, req.color.a);

		// Draw a pixel
		SDL_RenderPoint(renderer, req.pos.x, req.pos.y);

		// Reset color/alpha
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	}

	// Reset blend mode
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

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

	// Clear all "queues"
	UI_requests.clear();
	image_requests.clear();
	pixel_requests.clear();

	// Reset ImageManager's draw request ID counter
	req_id_counter = 0;

}

// Draw UI elements
void Renderer::DrawUI(const std::string &image_name, const int &pos_x, const int &pos_y) {

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
void Renderer::DrawUIEx(const std::string &image_name, const int &pos_x, const int &pos_y, const int &color_r, const int &color_g, const int &color_b, const int &color_a, const int &sorting_order) {

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
void Renderer::Draw(const std::string &image_name, const float &pos_x, const float &pos_y) {

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
void Renderer::DrawEx(const std::string &image_name, const float &pos_x, const float &pos_y, const int &rotation, const float &scale_x, const float &scale_y, const float &pivot_x, const float &pivot_y, const int &color_r, const int &color_g, const int &color_b, const int &color_a, const int &sorting_order) {

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
void Renderer::DrawPixel(const int &pos_x, const int &pos_y, const int &color_r, const int &color_g, const int &color_b, const int &color_a) {

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
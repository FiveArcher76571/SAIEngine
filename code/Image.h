// Image struct definition file (Image.h)
// Contains the Image type

#pragma once

#include <string>

#include "SDL_image/SDL_image.h"
#include "glm/glm.hpp"

// Container for images
struct Image {

	// Image name (filename minus extension)
	std::string name = "";

	// Image's SDL texture
	SDL_Texture *texture = nullptr;

};

// Container for Image draw requests
struct ImageDrawRequest {

	// Request ID
	int id = -1;

	// Image object
	Image *image = nullptr;

	// Position to draw
	SDL_FRect pos = { 0, 0, 0, 0 };

	// Rotation (deg)
	int rotation = 0;

	// Scale
	glm::vec2 scale = glm::vec2(1);

	// Pivot position
	SDL_FPoint pivot = { 0, 0 };

	// Color (SDL_Color)
	SDL_Color color = { 255, 255, 255, 255 };

	// Sorting order
	int sorting_order = 0;

};
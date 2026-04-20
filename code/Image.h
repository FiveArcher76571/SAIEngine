// Image struct definition file (Image.h)
// Contains the Image type

#pragma once

#include <string>

#include "SDL_image/SDL_image.h"

// Container for images
struct Image {

	// Image name (filename minus extension)
	std::string name = "";

	// Image's SDL texture
	SDL_Texture *texture = nullptr;

};
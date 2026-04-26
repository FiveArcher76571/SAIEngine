// ImageManager class definition file (ImageManager.cpp)
// Holds images and image-related functions

#include "ImageManager.h"

// Searches for and creates an Image object of the filename given
// If it has already been cached, return a pointer to that instead
Image *ImageManager::get_image(const std::string &filename, SDL_Renderer *renderer) {

	// First, check if the image has already been cached...
	if (images.find(filename) != images.end()) {

		// If it has, return a reference to the corresponding Image object
		return &images.at(filename);

	}

	// If not, go ahead and create a new one...

	// Create an empty Image object
	Image new_image;

	// Check if it exists in resources/images/
	if (!std::filesystem::exists("resources/images/" + filename + ".png")) {

		// If it doesn't, throw an error and exit
		std::cout << "error: missing image " << filename;
		exit(0);

	}

	// Also load texture into this game's SDL renderer
	SDL_Texture *image_texture = IMG_LoadTexture(renderer, ("resources/images/" + filename + ".png").c_str());

	// Now we know it exists, so store the full Image object into the list...

	new_image.name = filename;
	new_image.texture = image_texture;

	// Now add the finished image to the cache
	images.insert({ filename, new_image });

	// Return a reference to the inserted Image object
	return &images.at(filename);

}

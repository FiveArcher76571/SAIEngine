// Image manager class declaration file (ImageManager.h)
// Holds ImageManager class declaration

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include "Image.h"
#include "GameSetup.h"
#include "JSONReader.h"

#include "rapidjson/document.h"
#include "SDL3/SDL.h"
#include "SDL_image/SDL_image.h"

class ImageManager {

	// This game's SDL renderer
	SDL_Renderer *renderer = nullptr;

	// List of all intro image names
	std::vector<Image> intro_images;

	// Hashmap of Image objects, keyed by filename
	std::unordered_map<std::string, Image> images;

public:

	// Initialize the stored renderer pointer
	void initialize(SDL_Renderer *game_renderer);

	// Searches for and creates an Image object of the filename given
	// If it has already been cached, return a pointer to that instead
	Image *get_image(const std::string &filename);

};

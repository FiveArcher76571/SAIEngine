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

public:

	// Hashmap of Image objects, keyed by filename
	static inline std::unordered_map<std::string, Image> images;

	/////
	///// Functions
	/////

	// Searches for and creates an Image object of the filename given
	// If it has already been cached, return a pointer to that instead
	static Image *get_image(const std::string &filename, SDL_Renderer *renderer);

};

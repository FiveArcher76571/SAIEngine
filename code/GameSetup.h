// Game setup header file (GameSetup.h)
// Holds GameSetup class declaration

#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "JSONReader.h"

#include "rapidjson/document.h"

class GameSetup {

	// game.config rapidJSON document
	static inline rapidjson::Document game_config;

public:

	// GameSetup constructor
	// Reads in game.config from resources/
	GameSetup();

	// Check if the given parameter is defined in game.config
	static bool has_member(const std::string &parameter);

	// Get given parameter as a string
	static std::string get_string(const std::string &parameter);

	// Get given parameter as a float
	static float get_float(const std::string &parameter);

	// Load given parameter into the given rapidJSON Value array
	// Return false if it doesn't exist
	static bool load_array(const std::string &parameter, rapidjson::Value &value);	

	// Get initial scene name
	// Required to be in game.config
	static std::string get_initial_scene();

};

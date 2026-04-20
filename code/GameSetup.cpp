// Game setup definition file (GameSetup.cpp)
// Functions related to processing and pulling info from the config JSON files

#include "GameSetup.h"

// GameSetup constructor
// Reads in game.config from resources/
GameSetup::GameSetup() {
	
	// If the resources folder doesn't exist in the root directory, throw an error
	if (!std::filesystem::exists("resources/")) {

		std::cout << "error: resources/ missing";
		exit(0);

	}

	// game.config file path
	const char *game_config_path = "resources/game.config";

	// If game.config doesn't exist in the resources folder, throw an error
	if (!std::filesystem::exists(game_config_path)) {

		std::cout << "error: resources/game.config missing";
		exit(0);

	}

	// Read game.config
	JSONReader::read_json(game_config_path, game_config);

}

// Check if the given parameter is defined in game.config
bool GameSetup::has_member(const std::string &parameter) {

	return game_config.HasMember(parameter.c_str());

}

// Get given parameter as a string
// If it doesn't exist, return an empty string
std::string GameSetup::get_string(const std::string &parameter) {

	return game_config.HasMember(parameter.c_str()) ? game_config[parameter.c_str()].GetString() : "";

}

// Get given parameter as a float
float GameSetup::get_float(const std::string &parameter) {

	return game_config.HasMember(parameter.c_str()) ? game_config[parameter.c_str()].GetFloat() : 0.0f;

}

// Load given parameter into the given rapidJSON Value array
// Return false if it doesn't exist
bool GameSetup::load_array(const std::string &parameter, rapidjson::Value &value) {

	// Check if the member exists
	if (game_config.HasMember(parameter.c_str())) {

		// If so, get the array of strings
		value = game_config[parameter.c_str()].GetArray();

		return true;

	}

	// Otherwise, return false
	return false;

}

// Get initial scene name
// Required to be in game.config
std::string GameSetup::get_initial_scene() {

	// Throw error if initial scene has not been defined...
	if (!game_config.HasMember("initial_scene")) {

		std::cout << "error: initial_scene unspecified";
		exit(0);

	}

	return game_config["initial_scene"].GetString();

}

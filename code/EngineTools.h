// Various helpful engine-related functions (EngineTools.h)
// To be used primarily by Lua files

#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <algorithm>
#include "lua.hpp"
#include "LuaBridge.h"

// EngineTools class
class EngineTools {

	// Current frame tracker
	static inline int frame_number = 0;

public:

	/////
	///// Lua-exposed
	/////

	// Prints given message to cout (log)
	static inline void Log(const std::string &message) {

		std::cout << message << std::endl;

	}

	// Prints given message to cout (error log)
	static inline void LogError(const std::string &message) {

		std::cout << message << std::endl;

	}

	// Force quit the application
	static inline void Quit() {

		exit(0);

	}

	// Stall the program for a given amount of time (in milliseconds)
	static inline void Sleep(const int &ms) {

		std::this_thread::sleep_for(std::chrono::milliseconds(ms));

	}

	// Open a given URL link
	static inline void OpenURL(const std::string &url) {

#if defined _WIN32
		std::string cmd = "start " + url;
		std::system(cmd.c_str());
#elif defined __APPLE__
		std::string cmd = "open " + url;
		std::system(cmd.c_str());
#else
		std::string cmd = "xdg-open " + url;
		std::system(cmd.c_str());
#endif

	}

	// Get the current frame number
	static inline int GetFrameNumber() {

		return frame_number;

	}

	/////
	///// Internal use only
	/////

	// Reports Lua-side errors, correctly formatted
	// Taken from spec
	static inline void ReportError(const std::string &actor_name, const luabridge::LuaException &exception) {

		// Get the error message
		std::string error_message = exception.what();

		// Normalize file paths (for different platforms)
		std::replace(error_message.begin(), error_message.end(), '\\', '/');

		// Print the error with color coding
		std::cout << "\033[31m" << actor_name << " : " << error_message << "\033[0m" << std::endl;

	}

	// Updates the frame count (to be called every frame)
	static inline void UpdateFrame() {

		frame_number++;

	}

};
// JSON reader header file (JSONReader.h)
// Contains the JSON reader function

#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

class JSONReader {

public:

	// Reads a JSON file given a path, and outputs to a rapidJSON document reference
	// Returns true if the operation was successful, false otherwise
	static bool read_json(const char *path, rapidjson::Document &document) {

		// Check if the file exists in the filesystem
		if (!std::filesystem::exists(path)) return false;

		// JSON file pointer
		FILE *file_pointer = nullptr;

		// Assign the pointer to the file in the filesystem

#ifdef _WIN32
		fopen_s(&file_pointer, path, "rb");
#else
		file_pointer = std::fopen(path, "rb");
#endif

		// If the file pointer is still null, throw an error
		// This will never run because the open function will throw an error first
		// But it's to make the green squigglies happy
		if (file_pointer == nullptr) {
			std::cout << "Error: file does not exist";
			exit(0);
		}

		// Set document buffer
		char buffer[65536];

		// Create rapidJSON file read stream
		rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));

		// Parse the JSON data from the file into a rapidJSON document
		document.ParseStream(stream);

		// Close the file
		std::fclose(file_pointer);

		// Throw an error if there are any errors in the JSON file
		if (document.HasParseError()) {
			rapidjson::ParseErrorCode errorCode = document.GetParseError();
            std::cout << "error parsing json at [" << path << "]" << std::endl << errorCode << std::endl;
			exit(0);
		}

		return true;

	}

};

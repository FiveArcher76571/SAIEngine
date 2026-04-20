// Text manager class declaration file (TextManager.h)
// Holds the TextManager class declaration

#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <queue>
#include <iostream>

#include "rapidjson/document.h"
#include <SDL3/SDL.h>
#include <SDL_ttf/SDL_ttf.h>

// Container for rendering text
struct Text {

	// String of text
	std::string text = "";

	// Text position in window, as an SDL_FRect
	SDL_FRect position = { 0, 0, 0, 0 };

	// Font to use
	TTF_Font *font = nullptr;

	// Font size (default to 16)
	int font_size = 16;

	// Text color (default to white)
	SDL_Color color = { 255, 255, 255, 255 };

	// Loaded surface containing the text
	SDL_Surface *surface = nullptr;

};

class TextManager {

	// This game's font objects
	static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;

public:

	// Render queue for text objects
	static inline std::queue<Text> render_queue;

	// Initialize SDL_ttf
	static void initialize();

	// Get a font given its name
	static TTF_Font *get_font(const std::string &font_name, const int &font_size);

	// Queue a text rendering request
	static void CreateTextRenderRequest(const std::string &text, const int &x_pos, const int &y_pos, const std::string &font, const int &font_size, const Uint8 &color_r, const Uint8 &color_g, const Uint8 &color_b, const Uint8 &color_a);

};


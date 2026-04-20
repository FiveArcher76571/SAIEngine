// TextManager class definition file (TextManager.cpp)
// Holds functions related to rendering text

#include "TextManager.h"

// Initialize SDL_ttf
void TextManager::initialize() {

	// Initialize SDL_ttf, throw an error if failed
	if (!TTF_Init()) {

		std::cout << "Error: SDL_ttf couldn't initialize!\nError:\n" << SDL_GetError();
		exit(0);

	}

}

// Get a font given its name
TTF_Font *TextManager::get_font(const std::string &font_name, const int &font_size) {

	// Check if we have it alredy, and return that if so
	if (fonts.find(font_name) != fonts.end() && fonts[font_name].find(font_size) != fonts[font_name].end()) return fonts[font_name][font_size];

	// If not, try to load it...

	// Now, check if the defined font file exists in the filesystem...
	if (!std::filesystem::exists("resources/fonts/" + font_name + ".ttf")) {

		// If not, throw an error
		std::cout << "error: font " << font_name << " missing";
		exit(0);

	}

	// So now we have a font!

	// Create a TTF_Font object
	TTF_Font *new_font = TTF_OpenFont(("resources/fonts/" + font_name + ".ttf").c_str(), font_size);

	// Store it in the list
	fonts[font_name][font_size] = new_font;

	// Now return the font object
	return new_font;

}

// Queue a text rendering request
void TextManager::CreateTextRenderRequest(const std::string &text, const int &x_pos, const int &y_pos, const std::string &font, const int &font_size, const Uint8 &color_r, const Uint8 &color_g, const Uint8 &color_b, const Uint8 &color_a) {

	// Create a new Text object given this info
	Text new_text = { text, SDL_FRect(), get_font(font, font_size), font_size, {color_r, color_g, color_b, color_a}};

	// Temp w/h ints
	int new_w = 0;
	int new_h = 0;

	// Add width and height to the position rect
	TTF_GetStringSize(new_text.font, text.c_str(), 0, &new_w, &new_h);

	// Put the ints in the FRect
	new_text.position.x = static_cast<float>(x_pos);
	new_text.position.y = static_cast<float>(y_pos);
	new_text.position.w = static_cast<float>(new_w);
	new_text.position.h = static_cast<float>(new_h);

	// Create a surface for the Text object
	new_text.surface = TTF_RenderText_Solid(new_text.font, text.c_str(), 0, new_text.color);

	// Add this to the queue
	render_queue.push(new_text);

}

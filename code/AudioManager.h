// Audio manager class declaration file (AudioManager.h)
// Holds declaration for AudioManager class

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "LuaManager.h"
#include "RenderedAudio.h"
#include "SequencedAudio.h"

#include "fluidsynth.h"
#include "SDL_mixer/SDL_mixer.h"

class AudioManager {

	// Mixer
	static inline MIX_Mixer *mixer;

	// Rendered audio manager
	static inline RenderedAudio raudio;

	// Sequenced audio manager
	static inline SequencedAudio saudio;

public:

	// Load the intro BGM if it is defined in game.config
	static void initialize();

	// Update loop
	static void update();

};


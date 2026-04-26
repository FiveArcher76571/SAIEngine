// AudioManager class definition file (AudioManager.cpp)
// Holds audio loading/playback-related functions

#include "AudioManager.h"

// Load the intro BGM if it is defined in game.config
void AudioManager::initialize() {

	// Initialize SDL_mixer
	if (!MIX_Init()) {

		std::cout << "Error when calling MIX_Init()\n" << SDL_GetError();
		exit(0);

	}

	// Open the audio device to begin
	mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
	if (mixer == nullptr) {

		// If we couldn't open the audio device, throw an error
		std::cout << "error: unable to open audio device";
		exit(0);

	}

	// Initialize tracks to play audio on
	tracks.resize(50, nullptr);

}

// Load the given audio file(name) if it exists and return a MIX_Audio pointer
MIX_Audio *AudioManager::load_audio_file(const std::string &filename) {

	// The new MIX_Audio object
	MIX_Audio *new_audio = nullptr;

	// If we've already cached it, just return that
	if (audio_assets.find(filename) != audio_assets.end()) return audio_assets[filename];

	// Don't try to look if this audio file hasn't been defined
	if (filename != "") {

		// Now, check if the audio directory and the given audio file exist in the filesystem, and throw an error if not
		if (!std::filesystem::exists("resources/audio/") || (!std::filesystem::exists("resources/audio/" + filename + ".wav") && !std::filesystem::exists("resources/audio/" + filename + ".ogg"))) {

			std::cout << "error: failed to play audio clip " << filename;
			exit(0);

		}

		// Now we know the file exists...

		// Check the file extension for .wav or .ogg, and update the filename acordingly
		std::string full_file = "resources/audio/" + filename;
		full_file += std::filesystem::exists("resources/audio/" + filename + ".wav") ? ".wav" : ".ogg";

		// Load the audio file into the intro BGM Mix_Chunk pointer
		new_audio = MIX_LoadAudio(mixer, full_file.c_str(), false);

		// Check for errors
		if (new_audio == nullptr) {
			std::cout << "Error: failed to load audio file " << filename << "\n" << SDL_GetError();
			exit(0);
		}

	}

	// The file has definitely been loaded at this point...

	// Add it to our assets list
	audio_assets[filename] = new_audio;

	// Return the created audio Mix_Chunk
	return new_audio;

}

// Play/loop given audio in the given channel with/without looping
void AudioManager::Play(const int &channel, const std::string &trackname, const bool &loop) {

	// Load in the given file (if not already)
	MIX_Audio *audio = load_audio_file(trackname);

	// Play the file with given parameters
	int loops = loop ? -1 : 0;

	// If the channel given isn't defined (over 50, under 0), throw an error
	if (channel >= 50 || channel < 0) {

		std::cout << "Error: there are only 50 channels, defined from [0,50)";
		exit(0);

	}

	// Check if track has been defined, and initialize it if not
	if (tracks.at(channel) == nullptr) {

		tracks.at(channel) = MIX_CreateTrack(mixer);
		if (tracks.at(channel) == nullptr) {

			std::cout << "error: unable to create MIX_Track at channel " << channel;
			exit(0);

		}

	}

	MIX_SetTrackAudio(tracks.at(channel), audio);

	SDL_PropertiesID options = SDL_CreateProperties();

	SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loops);

	MIX_PlayTrack(tracks.at(channel), options);

	//AudioHelper::Mix_PlayChannel(channel, audio_assets.at(trackname), loops);

}

// Halt the given audio channel
void AudioManager::Halt(const int &channel) {

	//AudioHelper::Mix_HaltChannel(channel);

}

// Set the given channel's volume
void AudioManager::SetVolume(const int &channel, const int &volume) {

	//AudioHelper::Mix_Volume(channel, volume);

}
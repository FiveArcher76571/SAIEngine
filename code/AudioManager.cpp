// AudioManager class definition file (AudioManager.cpp)
// Holds audio loading/playback-related functions

#include "AudioManager.h"

// Load the intro BGM if it is defined in game.config
void AudioManager::initialize() {

	// Initialize SDL_mixer
	MIX_Init();

	// Open the audio device to begin
	mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

	if (mixer == nullptr) {

		// If we couldn't open the audio device, throw an error
		std::cout << "error: unable to open audio device";
		exit(0);

	}

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
		std::string full_file = filename;
		full_file += std::filesystem::exists("resources/audio/" + filename + ".wav") ? ".wav" : ".ogg";

		// Load the audio file into the intro BGM Mix_Chunk pointer
		new_audio = MIX_LoadAudio(mixer, ("resources/audio/" + full_file).c_str(), true);

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
	MIX_Audio *track = load_audio_file(trackname);

	// Play the file with given parameters
	int loops = loop ? -1 : 0;

	MIX_PlayAudio(mixer, track);

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
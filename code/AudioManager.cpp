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

	// Initialize vectors for number of tracks
	tracks.resize(50, nullptr);
	rendered_loop_points.resize(50);
	track_bpm.resize(50, -1);

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

}

// Pause the given channel
void AudioManager::Pause(const int &channel) {

	MIX_PauseTrack(tracks.at(channel));

}

// Pause the whole mixer (all tracks)
void AudioManager::PauseAll() {

	MIX_PauseAllTracks(mixer);

}

// Resume the given channel
void AudioManager::Resume(const int &channel) {

	MIX_ResumeTrack(tracks.at(channel));

}

// Resume the whole mixer (all tracks)
void AudioManager::ResumeAll() {

	MIX_ResumeAllTracks(mixer);

}

// Set the given channel's gain
void AudioManager::SetGain(const int &channel, const int &new_gain) {

	MIX_SetTrackGain(tracks.at(channel), new_gain);

}

// Set custom loop points for the given channel (in ms)
void AudioManager::SetLoopPoints(const int &channel, const int64_t &start_pos, const int64_t end_pos) {

	rendered_loop_points.at(channel).start_ms = start_pos;
	rendered_loop_points.at(channel).end_ms = end_pos;

}

// Reset (remove) loop points for the given channel
void AudioManager::ResetLoopPoints(const int &channel) {

	rendered_loop_points.at(channel).start_ms = -1;
	rendered_loop_points.at(channel).end_ms = -1;

}

// Set the BPM for the given channel
void AudioManager::SetBPM(const int &channel, const int &new_bpm) {

	track_bpm.at(channel) = new_bpm;

}

// Get the BPM for the given channel
int AudioManager::GetBPM(const int &channel) {

	return track_bpm.at(channel);

}

// Rendered audio manager class declaration file (RenderedAudio.h)
// Holds declaration for RenderedAudio class

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "LoopPoint.h"

#include "SDL_mixer/SDL_mixer.h"

class RenderedAudio {

    // Mixer
	static inline MIX_Mixer *mixer;

	// List of mixer tracks
	static inline std::vector<MIX_Track*> tracks;

	// Audio MIX_Audio storage
	static inline std::unordered_map<std::string, MIX_Audio*> audio_assets;

    // Loop point trackers
	static inline std::vector<LoopPoint> loop_points;

	// Track BPMs
	static inline std::vector<int> track_bpm;

	// Measure lengths for each track
	static inline std::vector<int> measure_lengths;

    // Load the given audio file(name) if it exists and return a MIX_Audio pointer
    static inline MIX_Audio *load_audio_file(const std::string &filename) {

        // The new MIX_Audio object
        MIX_Audio *new_audio = nullptr;

        // If the filename is blank, don't do anything
        if (filename.empty()) return new_audio;

        // If we've already cached it, just return that
        if (audio_assets.find(filename) != audio_assets.end()) return audio_assets[filename];

        // Now, check if the audio directory and the given audio file exist in the filesystem, and throw an error if not
        if (!std::filesystem::exists("resources/audio/") || (!std::filesystem::exists("resources/audio/" + filename + ".wav") && !std::filesystem::exists("resources/audio/" + filename + ".ogg"))) {

            std::cout << "Error: couldn't find audio file " << filename << ".wav/.ogg";
            return new_audio;

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
            return new_audio;
        }

        // The file has definitely been loaded at this point...

        // Add it to our assets list
        audio_assets[filename] = new_audio;

        // Return the created audio Mix_Chunk
        return new_audio;

    }

public:

    // Initialize mixer and vectors
	static void initialize(MIX_Mixer *main_mixer);

    // Update loop
    static void update();

	// Play/loop given audio in the given channel with/without looping
	static void Play(const int &channel, const std::string &trackname, const bool &loop);

	// Pause the given channel
	static void Pause(const int &channel);

	// Pause the whole mixer (all tracks)
	static void PauseAll();

	// Resume the given channel
	static void Resume(const int &channel);

	// Resume the whole mixer (all tracks)
	static void ResumeAll();

	// Set the given channel's gain
	static void SetGain(const int &channel, const int &new_gain);

	// Set custom loop points for the given channel (in ms)
	static void SetLoopPoints(const int &channel, const int64_t &start_pos, const int64_t end_pos);

	// Reset (remove) loop points for the given channel
	static void ResetLoopPoints(const int &channel);

	// Set the BPM for the given channel
	static void SetBPM(const int &channel, const int &new_bpm);

	// Get the BPM for the given channel
	static int GetBPM(const int &channel);

	// Set a measure length for the given channel
	static void SetMeasureLength(const int &channel, const int &length);

	// Get the measure length of the given channel
	static int GetMeasureLength(const int &channel);

	// Checks if the given channel has reached the start of a new measure (plus or minus the given buffer in ms)
	static bool IsOnBeat(const int &channel, const int64_t &buffer);

};
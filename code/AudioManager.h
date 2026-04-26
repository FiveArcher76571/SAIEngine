// Audio manager class declaration file (AudioManager.h)
// Holds declaration for AudioManager class

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "GameSetup.h"

#include "fluidsynth.h"
#include "SDL_mixer/SDL_mixer.h"

// Loop point tracker (for custom loop points)
struct LoopPoint {

	// Loop start (in ticks/beats)
	int start_tick = -1;

	// Loop start (in seconds)
	float start_sec = -1.0f;

	// Loop end (in ticks/beats)
	int end_tick = -1;

	// Loop end (in seconds)
	float end_sec = -1.0f;

};

class AudioManager {

	// Mixer
	static inline MIX_Mixer *mixer;

	// List of mixer tracks
	static inline std::vector<MIX_Track*> tracks;

	// Audio Mix_Chunk storage
	static inline std::unordered_map<std::string, MIX_Audio*> audio_assets;

	// FluidSynth Settings (if defined)
	static inline fluid_settings_t *settings;

	// FluidSynth Synth (if defined)
	static inline fluid_synth_t *synth;

	// FluidSynth MIDI player (if defined)
	static inline fluid_player_t *MIDIplayer;

	// FluidSynth audio driver
	static inline fluid_audio_driver_t *audio_driver;

	// FluidSynth MIDI router
	static inline fluid_midi_router_t *midi_router;

	// FluidSynth MIDI driver
	static inline fluid_midi_driver_t *midi_driver;

	// FluidSynth MIDI controller rules
	static inline fluid_midi_router_rule_t *midi_router_rules;

	// MIDI channel volume trackers
	static inline int channel_volumes[16] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

	// Flag for if there are changes to apply
	// Apply changes on frame 1 as well (right after OnStart())
	static inline bool changes = true;

	// Loop point tracker
	static inline LoopPoint loop;

public:

	// Load the intro BGM if it is defined in game.config
	void initialize();

	// Update pending changes
	static void update() {

		// Only apply if there are pending changes...
		if (changes) {

			// Update volumes
			for (int channel = 0; channel < 16; channel++) {

				fluid_synth_cc(synth, channel, 7, channel_volumes[channel]);

			}

			// Reset flag
			changes = false;

		}

		// Apply always...

		// Only do loop logic if it has been set (not -1)...
		if (loop.end_tick > -1) {

			// Check our current position in the track...
			int current_tick = fluid_player_get_current_tick(MIDIplayer);

			// Check if we're past the end tick
			if (current_tick >= loop.end_tick) {

				// If a start tick is defined, go there
				// Otherwise, go to the start of the track
				int dest_tick = loop.start_tick > -1 ? loop.start_tick : 0;
				fluid_player_seek(MIDIplayer, dest_tick);

			}

		}

	}

	// Calls to enable MIDI playback with soundfonts
	static void MIDI_EnablePlayback() {

		// Set up FluidSynth
		settings = new_fluid_settings();

		// Initialize the synth
		synth = new_fluid_synth(settings);

		// Initialize the MIDI player
		MIDIplayer = new_fluid_player(synth);

	}

	// Enable MIDI controller support
	// To be called at OnStart()
	static void MIDI_EnableControllerSupport() {

		// Configure MIDI driver for Windows (winmidi)
		fluid_settings_setstr(settings, "midi.driver", "winmidi");

		// Set up MIDI router and create MIDI driver
		midi_router = new_fluid_midi_router(settings, fluid_synth_handle_midi_event, synth);
		midi_driver = new_fluid_midi_driver(settings, fluid_midi_router_handle_midi_event, midi_router);

	}

	// Load the given audio file(name) if it exists and return a Mix_Chunk pointer
	static MIX_Audio *load_audio_file(const std::string &filename);

	// Play/loop given audio in the given channel with/without looping
	static void Play(const int &channel, const std::string &trackname, const bool &loop);

	// Halt the given audio channel
	static void Halt(const int &channel);

	// Set the given channel's volume
	static void SetVolume(const int &channel, const int &volume);

	// Load a MIDI file from resources/audio
	static void MIDI_LoadMIDI(const std::string &filename) {

		// Check if the audio directory and the given file exist in the filesystem, and throw an error if not
		if (!std::filesystem::exists("resources/audio/") || !std::filesystem::exists("resources/audio/" + filename + ".mid")) {
			std::cout << "Error: MIDI file " << filename << " does not exist in resources/audio";
			exit(0);
		}

		// If it does exist, update the filename
		std::string midi_filename = "resources/audio/" + filename + ".mid";

		// Add the file to the player
		fluid_player_add(MIDIplayer, midi_filename.c_str());

	}

	// Load a soundfont file from resources/audio/<name>/
	static void MIDI_LoadSF2(const std::string &filename) {

		// All related soundfont files must be put into a folder of the given name...

		// If there is no such folder, throw a warning but do not exit
		if (!std::filesystem::exists("resources/audio/" + filename + "/")) {

			std::cout << "Error: SF2 folder " << filename << " does not exist under resources/audio";
			exit(0);

		}

		// The directory exists!

		// Assign to a string
		std::string sf_filepath = "resources/audio/" + filename + "/";

		// Iterate through all the files contained...
		for (auto const &entry : std::filesystem::directory_iterator{ std::filesystem::path(sf_filepath) }) {

			// Only try to load the file if it's a valid soundfont file...
			if (entry.path().extension() != ".sf2") continue;

			// Get the filename
			std::string file = entry.path().string();

			// Load the soundfont
			fluid_synth_sfload(synth, file.c_str(), 1);

		}

		// Update the driver
		audio_driver = new_fluid_audio_driver(settings, synth);

	}

	// Play a loaded MIDI file with its loaded soundfont
	// Also set number of loops (-1 to infinite)
	static void MIDI_Play(const int &loops) {

		fluid_player_set_loop(MIDIplayer, loops);
		fluid_player_play(MIDIplayer);
		//fluid_player_join(MIDIplayer);

	}

	// Pause the MIDI player
	static void MIDI_Pause() {

		fluid_player_stop(MIDIplayer);

	}

	// Resume a PAUSED MIDI
	// Attempting to resume a MIDI that hasn't been loaded leads to undefined behaviour
	static void MIDI_Resume() {

		fluid_player_play(MIDIplayer);

	}

	// Set reverb level to the current MIDI track
	// Takes in a level parameter, if 0 turns reverb off
	// Level is a float between 0 and 1 inclusive
	static void MIDI_SetReverb(const float &level) {

		// If the level is 0, turn off reverb and return
		if (level == 0.0f) {

			fluid_synth_reverb_on(synth, -1, false);
			return;

		}

		// Otherwise, turn on reverb and set the level
		fluid_synth_reverb_on(synth, -1, true);
		fluid_synth_set_reverb_group_level(synth, -1, level);

		// Set other levels
		fluid_synth_set_reverb_group_damp(synth, -1, 0.5f);
		fluid_synth_set_reverb_group_roomsize(synth, -1, 0.5f);
		fluid_synth_set_reverb_group_width(synth, -1, 0.5f);

	}

	// Set gain level for the synth overall (all channels)
	// Takes in a gain level parameter
	// Must be a float between 0 and 10 inclusive (FluidSynth limits)
	static void MIDI_SetGain(const float &new_gain) {

		// WARNING: Setting gain over 1 is super loud, like crazy loud omg
		// Thus, I am restricting values to between 0 and 1 inclusive...
		if (new_gain > 1.0f) {
			std::cout << "WARNING: Setting gain to a value above 1.0 is dangerous to hearing! (I would know I tried it with headphones...)\nI recommend any further gain manipulation to be done within the MIDI file itself, rather than globally.\n";
			return;
		}

		fluid_synth_set_gain(synth, new_gain);

	}

	// Set a particular channel's volume
	// Takes in a channel (int, no. of channels, usually [0, 15]) and level (float, [0, 127])
	// Also, this assumes that the user knows which channels are mapped to what instrument, as they can be absolutely anything
	static void MIDI_SetChannelVolume(const int &channel, const int &level) {

		// Set the volume
		fluid_synth_cc(synth, channel, 7, level);

		// Update channel tracker
		channel_volumes[channel] = level;

		// Set changes flag
		changes = true;

	}

	// Set loop start and end points for the current MIDI track
	// Set in "metronome ticks" (basically beats)
	static void MIDI_SetLoopPoints(const int &start_tick, const int &end_tick) {

		// Update loop point object
		loop.start_tick = start_tick;
		loop.end_tick = end_tick;

	}

	// Reset loop points to real start/end
	// Shorthand for calling Audio.SetLoopPoints(-1, -1)
	static void MIDI_ResetLoopPoints() {

		// Set both to -1
		loop.start_tick = -1;
		loop.end_tick = -1;

	}

	// Send a MIDI control note on to the synth manually
	// Takes in MIDI key number, channel, and velocity at which to play it
	static void MIDI_SendKeyOn(const int &key, const int &channel, const int &velocity) {

		fluid_synth_noteon(synth, channel, key, velocity);

	}

	// Send a MIDI control note off to the synth manually
	// Takes in same as above but no need for velocity
	static void MIDI_SendKeyOff(const int &key, const int &channel) {

		fluid_synth_noteoff(synth, channel, key);

	}

};


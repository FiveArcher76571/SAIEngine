// Audio manager class declaration file (AudioManager.h)
// Holds declaration for AudioManager class

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "GameSetup.h"
#include "InputManager.h"

#include "fluidsynth.h"
#include "SDL_mixer/SDL_mixer.h"

// Loop point tracker (for custom loop points)
struct LoopPoint {

	// Loop start (in ticks/beats)
	int start_tick = -1;

	// Loop start (in milliseconds)
	int64_t start_ms = -1;

	// Loop end (in ticks/beats)
	int end_tick = -1;

	// Loop end (in milliseconds)
	int64_t end_ms = -1;

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

	// MIDI loop point tracker
	static inline LoopPoint midi_loop;

	// Loop point trackers (for rendered audio)
	static inline std::vector<LoopPoint> rendered_loop_points;

	// Track BPMs (for rendered audio)
	static inline std::vector<int> track_bpm;

	// Measure lengths of each track in rendered audio
	static inline std::vector<int> measure_lengths;

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

		// Check loop points for the MIDI track
		// Only do tick loop logic if it has been set (not -1)...
		if (midi_loop.end_tick > -1) {

			// Check our current position in the track...
			int current_tick = fluid_player_get_current_tick(MIDIplayer);

			// Check if we're past the end tick
			if (current_tick >= midi_loop.end_tick) {

				// If a start tick is defined, go there
				// Otherwise, go to the start of the track
				int dest_tick = midi_loop.start_tick > -1 ? midi_loop.start_tick : 0;
				fluid_player_seek(MIDIplayer, dest_tick);

			}

		}

		// For every rendered track, check loop points...
		for (int i = 0; i < rendered_loop_points.size(); i++) {

			// The current channel to look at
			LoopPoint &loop = rendered_loop_points.at(i);

			// Only do seconds loop logic if it has been set (not -1)
			if (loop.end_ms > -1) {

				// Check our current position in the track (in ms)
				int64_t current_pos = MIX_TrackFramesToMS(tracks.at(i), MIX_GetTrackPlaybackPosition(tracks.at(i)));

				// Check if we're past the end point
				if (current_pos >= loop.end_ms) {

					// If a start position is defined, go there
					// Otherwise, go to the start of the track
					int64_t dest_pos = loop.start_ms > -1 ? loop.start_ms : 0;
					MIX_SetTrackPlaybackPosition(tracks.at(i), MIX_TrackMSToFrames(tracks.at(i), dest_pos));

				}
				
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

// Windows (winmidi)
#if defined _WIN32

		// Configure MIDI driver
		fluid_settings_setstr(settings, "midi.driver", "winmidi");

// OSX (jack)
#elif defined __APPLE__

		// Configure MIDI driver
		fluid_settings_setstr(settings, "midi.driver", "jack");

// Linux (alsa_seq)
#else

		// Configure MIDI driver
		fluid_settings_setstr(settings, "midi.driver", "alsa_seq");

#endif

		// Set up MIDI router and create MIDI driver
		midi_router = new_fluid_midi_router(settings, fluid_synth_handle_midi_event, synth);
		midi_driver = new_fluid_midi_driver(settings, fluid_midi_router_handle_midi_event, midi_router);

	}

	// Enable MIDI controller routing to input manager
	// To be called at OnStart()
	static void MIDI_EnableRoutingAsInput() {

		// Windows (winmidi)
#if defined _WIN32

		// Configure MIDI driver
		fluid_settings_setstr(settings, "midi.driver", "winmidi");

// OSX (jack)
#elif defined __APPLE__

		// Configure MIDI driver
		fluid_settings_setstr(settings, "midi.driver", "jack");

// Linux (alsa_seq)
#else

		// Configure MIDI driver
		fluid_settings_setstr(settings, "midi.driver", "alsa_seq");

#endif

		// Set up MIDI router and create MIDI driver
		midi_router = new_fluid_midi_router(settings, fluid_synth_handle_midi_event, synth);
		midi_driver = new_fluid_midi_driver(settings, InputManager::update_states_midi, midi_router);

	}

	// Load the given audio file(name) if it exists and return a Mix_Chunk pointer
	static MIX_Audio *load_audio_file(const std::string &filename);

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

	/////
	///// MIDI Functions
	/////

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

	// Get the currently playing MIDI file's tempo in BPM (according to FluidSynth)
	static int MIDI_GetBPM() {

		// Check if anything is playing, and return -1 if not
		if (fluid_player_get_status(MIDIplayer) != fluid_player_status::FLUID_PLAYER_PLAYING) return -1;

		// Otherwise, return the BPM
		return fluid_player_get_bpm(MIDIplayer);		

	}

	// Set the currently playing MIDI file's tempo in BPM
	static void MIDI_SetBPM(const int &new_tempo) {

		// Call the FluidSynth function
		fluid_player_set_bpm(MIDIplayer, new_tempo);

	}

	// Get the currently playing MIDI track's current (tempo) tick
	static int MIDI_GetCurrentTick() {

		// Call the FluidSynth function
		return fluid_player_get_current_tick(MIDIplayer);

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
		midi_loop.start_tick = start_tick;
		midi_loop.end_tick = end_tick;

	}

	// Reset loop points to real start/end
	// Shorthand for calling Audio.SetLoopPoints(-1, -1)
	static void MIDI_ResetLoopPoints() {

		// Set both to -1
		midi_loop.start_tick = -1;
		midi_loop.end_tick = -1;

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


// Sequenced audio manager class declaration file (SequencedAudio.h)
// Holds declaration for SequencedAudio class

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "LoopPoint.h"
#include "InputManager.h"

#include "fluidsynth.h"
#include "SDL_mixer/SDL_mixer.h"

class SequencedAudio {

	// FluidSynth Settings
	static inline fluid_settings_t *settings;

	// FluidSynth Synth
	static inline fluid_synth_t *synth;

	// FluidSynth MIDI player
	static inline fluid_player_t *player;

	// FluidSynth audio driver
	static inline fluid_audio_driver_t *audio_driver;

	// FluidSynth MIDI router
	static inline fluid_midi_router_t *router;

	// FluidSynth MIDI driver
	static inline fluid_midi_driver_t *driver;

	// FluidSynth MIDI controller rules
	static inline fluid_midi_router_rule_t *router_rules;

	// MIDI channel volume trackers
	static inline int channel_volumes[16] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

	// Flag for if there are changes to apply
	// Apply changes on frame 1 as well (right after OnStart())
	static inline bool changes = true;

	// Loop point tracker
	static inline LoopPoint loop;

public:

	// Update loop
	static void update();

    // Enable MIDI playback with soundfonts
    static void Enable();

    // Enable MIDI controller support
    // To be called at OnStart()
    static void EnableControllerSupport();

    // Enable MIDI controller routing to input manager
    // To be called at OnStart()
    static void EnableRoutingAsInput();

    // Load a MIDI file from resources/audio
    static void LoadMIDI(const std::string &filename);

    // Load soundfont file(s) from resources/audio/<name>/
    static void LoadSF2(const std::string &filename);

    // Play a loaded MIDI file with its loaded soundfont
    // Also set number of loops (-1 to infinite)
    static void Play(const int &loops);

    // Pause the MIDI player
    static void Pause();

    // Resume the MIDI player
    // Attempting to resume a MIDI that hasn't been loaded leads to undefined behaviour
    static void Resume();

    // Get the currently playing MIDI file's tempo in BPM (according to FluidSynth)
    static int GetBPM();

    // Set the currently playing MIDI file's tempo in BPM
    static void SetBPM(const int &new_tempo);

    // Get the currently playing MIDI track's current (tempo) tick
    static int GetCurrentTick();

    // Set reverb level to the current MIDI track
    // Takes in a level parameter, if 0 turns reverb off
    // Level is a float between 0 and 1 inclusive
    static void SetReverb(const float &level);

    // Set gain level for the synth overall (all channels)
    // Takes in a gain level parameter
    // Must be a float between 0 and 10 inclusive (FluidSynth limits)
    static void SetGain(const float &new_gain);

    // Set a particular channel's volume
    // Takes in a channel (int, no. of channels, usually [0, 15]) and level (float, [0, 127])
    // Also, this assumes that the user knows which channels are mapped to what instrument, as they can be absolutely anything
    static void SetChannelVolume(const int &channel, const int &level);

    // Set loop start and end points for the current MIDI track
    // Set in "metronome ticks" (basically beats)
    static void SetLoopPoints(const int &start_tick, const int &end_tick);

    // Reset loop points to real start/end
    // Shorthand for calling Audio.SetLoopPoints(-1, -1)
    static void ResetLoopPoints();

    // Send a MIDI control note on to the synth manually
    // Takes in MIDI key number, channel, and velocity at which to play it
    static void SendKeyOn(const int &key, const int &channel, const int &velocity);

    // Send a MIDI control note off to the synth manually
    // Takes in same as above but no need for velocity
    static void SendKeyOff(const int &key, const int &channel);

};


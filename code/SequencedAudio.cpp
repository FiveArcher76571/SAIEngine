// Sequenced audio class definition file (SequencedAudio.cpp)
// Holds sequenced audio playback-related functions

#include "SequencedAudio.h"

// Update loop
void SequencedAudio::update() {

    // Only apply if there are pending changes...
    if (changes) {

        // Update volumes
        for (int channel = 0; channel < 16; channel++) {

            fluid_synth_cc(synth, channel, 7, channel_volumes[channel]);

        }

        // Reset flag
        changes = false;

    }

    // Check loop points for the MIDI track
    // Only do tick loop logic if it has been set (not -1)...
    if (loop.end > -1) {

        // Check our current position in the track...
        int current_tick = fluid_player_get_current_tick(player);

        // Check if we're past the end tick
        if (current_tick >= loop.end) {

            // If a start tick is defined, go there
            // Otherwise, go to the start of the track
            int dest_tick = loop.start > -1 ? loop.start : 0;
            fluid_player_seek(player, dest_tick);

        }

    }

}

// Enable MIDI playback with soundfonts
void SequencedAudio::Enable() {

    // Set up FluidSynth
    settings = new_fluid_settings();

    // Initialize the synth
    synth = new_fluid_synth(settings);

    // Initialize the MIDI player
    player = new_fluid_player(synth);

}

// Enable MIDI controller support
// To be called at OnStart()
void SequencedAudio::EnableControllerSupport() {

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
    router = new_fluid_midi_router(settings, fluid_synth_handle_midi_event, synth);
    driver = new_fluid_midi_driver(settings, fluid_midi_router_handle_midi_event, router);

}

// Enable MIDI controller routing to input manager
// To be called at OnStart()
void SequencedAudio::EnableRoutingAsInput() {

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
    router = new_fluid_midi_router(settings, fluid_synth_handle_midi_event, synth);
    driver = new_fluid_midi_driver(settings, InputManager::update_states_midi, router);

}

// Load a MIDI file from resources/audio
void SequencedAudio::LoadMIDI(const std::string &filename) {

    // Check if the audio directory and the given file exist in the filesystem, and throw an error if not
    if (!std::filesystem::exists("resources/audio/") || !std::filesystem::exists("resources/audio/" + filename + ".mid")) {
        std::cout << "Error: MIDI file " << filename << ".mid does not exist in resources/audio";
        exit(0);
    }

    // If it does exist, update the filename
    std::string midi_filename = "resources/audio/" + filename + ".mid";

    // Add the file to the player
    fluid_player_add(player, midi_filename.c_str());

}

// Load soundfont file(s) from resources/audio/<name>/
void SequencedAudio::LoadSF2(const std::string &filename) {

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
void SequencedAudio::Play(const int &loops) {

    fluid_player_set_loop(player, loops);
    fluid_player_play(player);

}

// Pause the MIDI player
void SequencedAudio::Pause() {

    fluid_player_stop(player);

}

// Resume the MIDI player
// Attempting to resume a MIDI that hasn't been loaded leads to undefined behaviour
void SequencedAudio::Resume() {

    fluid_player_play(player);

}

// Get the currently playing MIDI file's tempo in BPM (according to FluidSynth)
int SequencedAudio::GetBPM() {

    // Check if anything is playing, and return -1 if not
    if (fluid_player_get_status(player) != fluid_player_status::FLUID_PLAYER_PLAYING) return -1;

    // Otherwise, return the BPM
    return fluid_player_get_bpm(player);		

}

// Set the currently playing MIDI file's tempo in BPM
void SequencedAudio::SetBPM(const int &new_tempo) {

    // Call the FluidSynth function
    fluid_player_set_bpm(player, new_tempo);

}

// Get the currently playing MIDI track's current (tempo) tick
int SequencedAudio::GetCurrentTick() {

    // Call the FluidSynth function
    return fluid_player_get_current_tick(player);

}

// Set reverb level to the current MIDI track
// Takes in a level parameter, if 0 turns reverb off
// Level is a float between 0 and 1 inclusive
void SequencedAudio::SetReverb(const float &level) {

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
void SequencedAudio::SetGain(const float &new_gain) {

    // WARNING: Setting gain over 1 is super loud, like crazy loud omg
    // Thus, I am restricting values to between 0 and 1 inclusive...
    if (new_gain > 1.0f) {
        std::cout << "WARNING: Setting gain to a value above 1.0 is dangerous to hearing! (I would know I tried it with headphones...)\nI recommend any further gain manipulation to be done within the MIDI file itself, rather than globally.\n";
    }

    fluid_synth_set_gain(synth, new_gain);

}

// Set a particular channel's volume
// Takes in a channel (int, no. of channels, usually [0, 15]) and level (float, [0, 127])
// Also, this assumes that the user knows which channels are mapped to what instrument, as they can be absolutely anything
void SequencedAudio::SetChannelVolume(const int &channel, const int &level) {

    // Set the volume
    fluid_synth_cc(synth, channel, 7, level);

    // Update channel tracker
    channel_volumes[channel] = level;

    // Set changes flag
    changes = true;

}

// Set loop start and end points for the current MIDI track
// Set in "metronome ticks" (basically beats)
void SequencedAudio::SetLoopPoints(const int &start_tick, const int &end_tick) {

    // Update loop point object
    loop.start = start_tick;
    loop.end = end_tick;

}

// Reset loop points to real start/end
// Shorthand for calling Audio.SetLoopPoints(-1, -1)
void SequencedAudio::ResetLoopPoints() {

    // Set both to -1
    loop.start = -1;
    loop.end = -1;

}

// Send a MIDI control note on to the synth manually
// Takes in MIDI key number, channel, and velocity at which to play it
void SequencedAudio::SendKeyOn(const int &key, const int &channel, const int &velocity) {

    fluid_synth_noteon(synth, channel, key, velocity);

}

// Send a MIDI control note off to the synth manually
// Takes in same as above but no need for velocity
void SequencedAudio::SendKeyOff(const int &key, const int &channel) {

    fluid_synth_noteoff(synth, channel, key);

}

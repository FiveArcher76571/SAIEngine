// Rendered audio class definition file (RenderedAudio.cpp)
// Holds rendered audio playback-related functions

#include "RenderedAudio.h"

// Initialize mixer and vectors
void RenderedAudio::initialize(MIX_Mixer *main_mixer) {

    // Get mixer
    mixer = main_mixer;

	// Initialize vectors for number of tracks
	tracks.resize(50, nullptr);
	loop_points.resize(50);
	track_bpm.resize(50, -1);
	measure_lengths.resize(50, 0);

}

// Update loop
void RenderedAudio::update() {

    // For every track, check loop points...
    for (int i = 0; i < loop_points.size(); i++) {

        // The current channel to look at
        LoopPoint &loop = loop_points.at(i);

        // Only do seconds loop logic if it has been set (not -1)
        if (loop.end > -1) {

            // Check our current position in the track (in ms)
            int64_t current_pos = MIX_TrackFramesToMS(tracks.at(i), MIX_GetTrackPlaybackPosition(tracks.at(i)));

            // Check if we're past the end point
            if (current_pos >= loop.end) {

                // If a start position is defined, go there
                // Otherwise, go to the start of the track
                int dest_pos = loop.start > -1 ? loop.start : 0;
                MIX_SetTrackPlaybackPosition(tracks.at(i), MIX_TrackMSToFrames(tracks.at(i), dest_pos));

            }
            
        }

    }

}

// Play/loop given audio in the given channel with/without looping
void RenderedAudio::Play(const int &channel, const std::string &trackname, const bool &loop) {

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
void RenderedAudio::Pause(const int &channel) {

	MIX_PauseTrack(tracks.at(channel));

}

// Pause the whole mixer (all tracks)
void RenderedAudio::PauseAll() {

	MIX_PauseAllTracks(mixer);

}

// Resume the given channel
void RenderedAudio::Resume(const int &channel) {

	MIX_ResumeTrack(tracks.at(channel));

}

// Resume the whole mixer (all tracks)
void RenderedAudio::ResumeAll() {

	MIX_ResumeAllTracks(mixer);

}

// Set the given channel's gain
void RenderedAudio::SetGain(const int &channel, const int &new_gain) {

	MIX_SetTrackGain(tracks.at(channel), new_gain);

}

// Set custom loop points for the given channel (in ms)
void RenderedAudio::SetLoopPoints(const int &channel, const int64_t &start_pos, const int64_t end_pos) {

	loop_points.at(channel).start = start_pos;
	loop_points.at(channel).end = end_pos;

}

// Reset (remove) loop points for the given channel
void RenderedAudio::ResetLoopPoints(const int &channel) {

	loop_points.at(channel).start = -1;
	loop_points.at(channel).end = -1;

}

// Set the BPM for the given channel
void RenderedAudio::SetBPM(const int &channel, const int &new_bpm) {

	track_bpm.at(channel) = new_bpm;

}

// Get the BPM for the given channel
int RenderedAudio::GetBPM(const int &channel) {

	return track_bpm.at(channel);

}

// Set a measure length for the given channel
void RenderedAudio::SetMeasureLength(const int &channel, const int &length) {

	measure_lengths.at(channel) = length;

}

// Get the measure length of the given channel
int RenderedAudio::GetMeasureLength(const int &channel) {

	return measure_lengths.at(channel);

}

// Checks if the given channel has reached the start of a new measure (plus or minus the given buffer in ms)
bool RenderedAudio::IsNewMeasure(const int &channel, const int &buffer) {

	// If neither BPM nor measure length have been set for this channel, return false
	if (!(track_bpm.at(channel) > -1 && measure_lengths.at(channel) > 0)) return false;

	// Get the current position of the track's audio in ms
	int64_t current_pos = MIX_TrackFramesToMS(tracks.at(channel), MIX_GetTrackPlaybackPosition(tracks.at(channel)));

	// Calculate the measure length in ms
	// (60,000ms <1 min> * measure length ms) / BPM
	int measure_length = (60000 * measure_lengths.at(channel)) / track_bpm.at(channel);

	// Get the distance between the current position and the most recent measure (current_pos % measure_length)
	int distance = current_pos % measure_length;

	// Return true if the distance is less than the given buffer amt (current pos within boundary)
	return distance <= buffer;

}

// Checks if the given channel has begun a new beat on this frame (plus or minus the given buffer in ms)
bool RenderedAudio::IsOnBeat(const int &channel, const int &buffer) {

	// If the BPM hasn't been set for this channel, return false
	if (track_bpm.at(channel) == -1) return false;

	// Get the current position of the track's audio in ms
	int64_t current_pos = MIX_TrackFramesToMS(tracks.at(channel), MIX_GetTrackPlaybackPosition(tracks.at(channel)));

	// Calculate beat length in ms
	// 60,000ms <1 min> / BPM
	int beat_length = 60000 / track_bpm.at(channel);

	// Get the distance between the current position and the most recent beat (current_pos % beat_length)
	int distance = current_pos % beat_length;

	// Return true if the distance is less than the given buffer amt (current pos within boundary)
	return distance <= buffer;

}

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
		std::cout << "Error: unable to open audio device\n" << SDL_GetError();
		exit(0);

	}

	// Initialize the rendered audio manager
	raudio.initialize(mixer);

	// Expose RAudio and SAudio functions to Lua...

	// Rendered Audio
	luabridge::getGlobalNamespace(LuaManager::get_lua_state())
		.beginNamespace("RAudio")
		.addFunction("Play", &RenderedAudio::Play)
		.addFunction("Pause", &RenderedAudio::Pause)
		.addFunction("PauseAll", &RenderedAudio::PauseAll)
		.addFunction("Resume", &RenderedAudio::Resume)
		.addFunction("ResumeAll", &RenderedAudio::ResumeAll)
		.addFunction("SetGain", &RenderedAudio::SetGain)
		.addFunction("SetLoopPoints", &RenderedAudio::SetLoopPoints)
		.addFunction("ResetLoopPoints", &RenderedAudio::ResetLoopPoints)
		.addFunction("SetBPM", &RenderedAudio::SetBPM)
		.addFunction("GetBPM", &RenderedAudio::GetBPM)
		.addFunction("SetMeasureLength", &RenderedAudio::SetMeasureLength)
		.addFunction("GetMeasureLength", &RenderedAudio::GetMeasureLength)
		.addFunction("IsNewMeasure", &RenderedAudio::IsNewMeasure)
		.endNamespace();

	// Sequenced Audio
	luabridge::getGlobalNamespace(LuaManager::get_lua_state())
		.beginNamespace("SAudio")
		.addFunction("Enable", &SequencedAudio::Enable)
		.addFunction("EnableMIDIController", &SequencedAudio::EnableControllerSupport)
		.addFunction("LoadMIDI", &SequencedAudio::LoadMIDI)
		.addFunction("LoadSF2", &SequencedAudio::LoadSF2)
		.addFunction("Play", &SequencedAudio::Play)
		.addFunction("Pause", &SequencedAudio::Pause)
		.addFunction("Resume", &SequencedAudio::Resume)
		.addFunction("GetCurrentTick", &SequencedAudio::GetCurrentTick)
		.addFunction("GetBPM", &SequencedAudio::GetBPM)
		.addFunction("SetBPM", &SequencedAudio::SetBPM)
		.addFunction("SetReverb", &SequencedAudio::SetReverb)
		.addFunction("SetGain", &SequencedAudio::SetGain)
		.addFunction("SetChannelVolume", &SequencedAudio::SetChannelVolume)
		.addFunction("SetLoopPoints", &SequencedAudio::SetLoopPoints)
		.addFunction("ResetLoopPoints", &SequencedAudio::ResetLoopPoints)
		.addFunction("SendKeyOn", &SequencedAudio::SendKeyOn)
		.addFunction("SendKeyOff", &SequencedAudio::SendKeyOff)
		.endNamespace();

}

// Update loop
void AudioManager::update() {

	// Update both audio managers
	raudio.update();
	saudio.update();

}
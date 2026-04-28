# SAIEngine
A minimalist 2D game engine with robust audio playback and synthesis

## Details
- Uses a C++ backend with Lua as a scripting language (using LuaBridge)
- Employs a Unity-like component and scene system
  - This means actors are flexible and modular, and scene management is straightforward!
- Uses SDL3 for rendering, input, image/text handling
- Uses GLM for efficient vector math
- Uses FluidSynth for MIDI playback/controller support and SoundFont support
- Is kinda cool I think

## Usage Instructions
To use my engine, simply [download the latest compiled binary here](https://github.com/FiveArcher76571/SAIEngine/releases/latest) or build it yourself (see below). Once you do that do the following...
- Create assets, components, and scenes in a `resources` folder, placed next to the executable
- Run the executable
- ...
- Profit (if you wanna)!

## Build Instructions
The files are all there for you to add to your workflow as you please! However, I also provide a (cobbled together) CMake file for you to use. Simply run `cmake .` in your desired terminal and watch it do its magic!

### Note on CMake
As of April 28th 2026, I've only made my `CMakeLists.txt` file to work with Windows (specifically tested with Visual Studio 2022). I plan to add macOS and Linux support soon, but for now sorry for the inconvenience!
# SAIEngine
A minimalist 2D game engine with robust audio playback and synthesis

## Details
- Uses a C++ backend with Lua as a scripting language (using LuaBridge)
- Employs a Unity-like component and scene system
  - This means actors are flexible and modular, and scene management is straightforward!
- Uses SDL3 for rendering, input, image/text handling
- Uses GLM for efficient vector math
- Uses FluidSynth for MIDI playback/controller support and SoundFont support
- Compatible with Windows, Mac, and Linux systems
- Is kinda cool I think :)

## Usage Instructions
To use my engine, simply [download the latest compiled binary here](https://github.com/FiveArcher76571/SAIEngine/releases/latest) or build it yourself (see below). Once you do that do the following...
- Create assets, components, and scenes in a `resources` folder, placed next to the executable
- Run the executable*
- ...
- Profit (if you wanna)!

### *Running on Mac
Currently, though SAIEngine will compile to a .app bundle (which can usually be double-clicked to launch from the GUI), the app can only be launched manually via the Terminal. To launch, simply run the following command from a Terminal window in the same directory as the built app:

```./SAIEngine/Contents/MacOS/SAIEngine```

## Build Instructions
The files are all there for you to add to your workflow as you please! However, I also provide a (cobbled together) CMake file for you to use. Simply run `cmake .` in your desired terminal and build using your desired workflow!

### Compatible Platforms
SAIEngine is prepared to, and (hopefully) will, work on Windows, Mac, and Linux machines! Below are the specific platforms I've tested on myself (the platforms I have access to), but it *should* work on other platforms as well.
- Windows
  - Windows 11 x64
  - Visual Studio 2022
- Mac
  - MacOS 15 Sequoia on M1
  - AppleClang/Xcode
- Linux
  - Debian 13 x64
  - Clang/++
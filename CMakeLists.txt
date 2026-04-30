cmake_minimum_required(VERSION 3.10)
project(SAIEngine)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable C for Lua
enable_language(C)

# C++ compiler

# MSVC for Windows
if (WIN32)

    set(CMAKE_C_COMPILER "cl.exe")
    set(CMAKE_CXX_COMPILER "cl.exe")

    # Suppress deprecation warnings and add optimization O3
    add_compile_options(/O2 /wd4996)

endif(WIN32)

# Clang for Linux
if (UNIX)

    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)

    # Suppress deprecation warnings and add optimization O3
    add_compile_options(-O3 -Wno-deprecated-declarations)

endif (UNIX)

# My source files
file(GLOB CPP_SOURCE "code/*.cpp")

# My own headers
include_directories(code/)

# Include headers
include_directories(include/fluidsynth/)
include_directories(include/glm/)
include_directories(include/LuaBridge/)
include_directories(include/rapidjson/)
include_directories(include/SDL_image/)
include_directories(include/SDL_mixer/)
include_directories(include/SDL_ttf/)
include_directories(include/SDL3/)
include_directories(include/)

# Lua source files
file(GLOB LUA_SOURCE "lua_5.4.6/src/*.c")

# Lua includes
include_directories(lua_5.4.6/inc/)

# Executable: game_engine
add_executable(SAIEngine ${CPP_SOURCE} ${LUA_SOURCE})

# Get the libraries needed

# Define directories
target_link_directories(SAIEngine
    PRIVATE
    libraries/SDL3
    libraries/SDL3_image
    libraries/SDL3_mixer
    libraries/SDL3_ttf
    libraries/FluidSynth
)


target_link_libraries(SAIEngine
    PRIVATE
    SDL3
    SDL3_image
    SDL3_mixer
    SDL3_ttf
    fluidsynth
)

# Copy over DLLs to build directory (Windows)
if (WIN32)

    file(GLOB DLL_FILES "libraries/_dll/*.dll")
    foreach(DLL ${DLL_FILES})
        add_custom_command(TARGET SAIEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL}" 
            $<TARGET_FILE_DIR:SAIEngine>
        )
    endforeach()
    
endif (WIN32)

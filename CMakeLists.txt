cmake_minimum_required(VERSION 3.17)
project(SAIEngine)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable C for Lua
enable_language(C)

# C++ compiler (including optimization and no deprecation warnings)

# MSVC for Windows
if (WIN32)
    set(CMAKE_C_COMPILER "cl.exe")
    set(CMAKE_CXX_COMPILER "cl.exe")
    add_compile_options(/O2 /wd4996)
endif()

# Clang for Linux
if (UNIX AND NOT APPLE)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
    add_compile_options(-O3 -Wno-deprecated-declarations)
endif ()

# Apple Clang for macOS (default for OS)
if (APPLE)
    add_compile_options(-O3 -Wno-deprecated-declarations)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version")
endif ()

# My source files
file(GLOB CPP_SOURCE "code/*.cpp")

# Lua source files
file(GLOB LUA_SOURCE "lua_5.4.6/src/*.c")

# Executable: game_engine
add_executable(SAIEngine ${CPP_SOURCE} ${LUA_SOURCE})

# My own headers
target_include_directories(SAIEngine
    PRIVATE
    code/
)

# External include headers
target_include_directories(SAIEngine
    PRIVATE
    include/fluidsynth/
    include/glm/
    include/LuaBridge/
    include/rapidjson/
    include/SDL_image/
    include/SDL_mixer/
    include/SDL_ttf/
    include/SDL3/
    include/
)

# Lua includes
target_include_directories(SAIEngine
    PRIVATE
    lua_5.4.6/inc/
)

# Get the libraries needed...

# Define library directory based on platform

# Windows
if (WIN32)
    set(LIB_DIR "libraries/Windows")
endif ()

# Linux
if (UNIX AND NOT APPLE)
    set(LIB_DIR "libraries/Linux")
endif ()

# OSX
if (APPLE)
    set(LIB_DIR "libraries/OSX")
endif ()

# Define directories
target_link_directories(SAIEngine
    PRIVATE
    ${LIB_DIR}/SDL3
    ${LIB_DIR}/SDL3_image
    ${LIB_DIR}/SDL3_mixer
    ${LIB_DIR}/SDL3_ttf
    ${LIB_DIR}/FluidSynth
)

target_link_libraries(SAIEngine
    PRIVATE
    SDL3
    SDL3_image
    SDL3_mixer
    SDL3_ttf
    fluidsynth
)

# Copy over DLLs to build directory (Windows only)
if (WIN32)
    file(GLOB DLL_FILES "libraries/Windows/_dll/*.dll")
    foreach(DLL ${DLL_FILES})
        add_custom_command(TARGET SAIEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL}" 
            $<TARGET_FILE_DIR:SAIEngine>
        )
    endforeach()
endif (WIN32)

# Add build data to build to an app, and create executable script to launch with resources dir (OSX only)
if (APPLE)
    set_target_properties(SAIEngine PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "SAIEngine"
        MACOSX_BUNDLE_BUNDLE_VERSION "0.3"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.3"
        MACOSX_BUNDLE_COPYRIGHT "Saicharan Vemuri"
        XCODE_SCHEME_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif ()
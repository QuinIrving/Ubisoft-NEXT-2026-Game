
# Living Cube - Ubisoft NEXT 2026 Submission

## Overview

**Living Cube** is a high speed survival platformer submitted for the Ubisoft NEXT 2026 Programming Challenge.
Set entirely within the bounds of a single containment cube, the objective is to survive as long as possible against swarms of seeking entities. The project was built using a custom C++ 3D engine that handles the entire transformation pipeline, passing only final interpolated vertex data to the provided API's primitive triangle drawer.
The game combines Quake-style movement mechanics (Bunny Hopping, Airstrafing, Surfing) with procedural platform generation, requiring the player to maintain momentum to outrun intelligent Boid swarms.



# Ubi Contest API for Mac \& Windows

## Dependencies

### Windows
* Visual Studio 2022 Community https://visualstudio.microsoft.com/vs/community/
    * This is a windows IDE, windows build system and c++ compiler all bundled together
* CMake (for build system) https://cmake.org/
    * This is a build system generator, which allows us to build the project on multiple platforms

### MacOS
* Homebrew (for package management) [get from homebrew website] https://brew.sh/
    * This is a package manager for macos systems
* CMake (see windows section)
* freeglut
    * This is an OpenGL utility library. Install by running [brew install freeglut] in a macos terminal
* SDL3
    * This is a cross-platform utility library. Install by running [brew install SDL3] in a macos terminal
* XCode command line tools
    * This has the MacOS build system and compiler. It should be installed automatically when installing homebrew, but can be installed through the MacOS terminal.

## To build

### Windows
* Run generate-windows.bat script in DAU-NEXT-API directory
* open visual studio solution in build/win64
* build and run from visual studio

### MacOS
* Run ./generate-macos.sh script in DAU-NEXT-API directory
* go to build/macos directory
    * run [make all] in macos terminal to build the program
    * run [make run] to run the game
        * Note that the application can't be closed in the window. use cmd + Q to quit the program
* If using VSCode, commands can be run from the VSCode terminal

## To add code to project
* Add new code files in src/Game subdirectory
* Re-run the generate-windows or generate-macos script

## Useful Notes
* When run using the generated projects, the game will run in the DAU-NEXT-API directory, which is useful for referencing data files.

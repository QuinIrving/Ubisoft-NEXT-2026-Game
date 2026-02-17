
# Living Cube - Ubisoft NEXT 2026 Submission

## Overview

**Living Cube** is a high speed survival platformer submitted for the Ubisoft NEXT 2026 Programming Challenge.

Set entirely within the bounds of a single containment cube, the objective is to survive as long as possible against swarms of seeking entities. The project was built using a custom C++ 3D engine that handles the entire transformation pipeline, passing only final interpolated vertex data to the provided API's primitive triangle drawer.

The game combines Quake-style movement mechanics (Bunny Hopping, Airstrafing, Surfing) with procedural platform generation, requiring the player to maintain momentum to outrun targeted Boid swarms.

## Game Demo:


## Core Systems
### 1. Advanced Movement Physics

The player controller implements kinematic movement heavily inspired by the Source Engine/Quake. It allows for gaining velocity through strafing and surfing ramps.

    Air Strafing: Projecting the velocity vector onto the wish-direction to allow turning in mid-air without losing speed.

    Friction & Ground Control: Custom friction application that differentiates between ground traction and air resistance.

    Surfing: Handling collision responses on ramp surfaces to slide rather than stop.

### 2. Continuous Collision Detection (CCD)

To support high-velocity gameplay without "tunneling" (passing through objects), the engine utilizes Continuous Collision Detection.

    Ray vs. OBB (Oriented Bounding Box): Used for camera checks and high-speed projectile logic.

    AABB vs. OBB: Utilized for player interaction with rotating rectangles and procedural platforms.

    Separating Axis Theorem (SAT): Implemented for precise intersection tests between the player and the moving geometry.

### 3. AI Swarm (Boids)

Enemies utilize a modified Reynolds' Boids algorithm. They do not pathfind, but rather steer.

    Cohesion: Boids move toward the center of mass of their neighbors.

    Separation: Boids steer to avoid crowding neighbors.

    Alignment: Boids steer towards the average heading of neighbors.

    Targeting: A fourth vector added to the steering force to relentlessly pursue the player.

## Architecture

The provided API for this challenge was limited to drawing colored triangles. I implemented a complete graphics pipeline to bridge the gap:

    Scene Graph: Manages World-Space transformations for the player, platforms, and boids.

    Transformation Pipeline:

        Calculates Model-View-Projection (MVP) matrices.

        Transforms vertices from Local Space → Clip Space.

    Rasterization Prep: Sends processed vertices with interpolated colours to the API.

    Note: The visual style (Solid Colors, No Textures) is a deliberate aesthetic choice, mimicking early 3D debug views to focus purely on gameplay fluidity and geometric clarity.

## Controls

    WASD: Movement (relative to camera).

    Mouse: Look.

    Space: Jump (Hold to Bunny Hop).

    Strafe Keys (A/D) + Mouse Turn: Gain speed in air.


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

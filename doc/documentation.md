# Arcade Implementation Guide

This document explains how to implement new graphic libraries or game libraries compatible with the Arcade system.

## Table of Contents
1. [Project Overview](#project-overview)
2. [Directory Structure](#directory-structure)
3. [Adding a New Graphics Library](#adding-a-new-graphics-library)
4. [Adding a New Game](#adding-a-new-game)
5. [Building and Compiling](#building-and-compiling)
6. [Interface Compatibility](#interface-compatibility)

## Project Overview

Arcade is a gaming platform that lets users choose a game to play and keeps track of scores. All graphics libraries and games are implemented as dynamic libraries that are loaded at runtime.

The system is built around three main components:
- **Core**: The main executable that loads and manages libraries
- **Graphics Libraries**: Display modules that handle rendering and input
- **Game Libraries**: Game modules that implement game logic

## Directory Structure

```
./
├── assets/            # Game assets (fonts, textures, etc.)
├── doc/               # Documentation
├── lib/               # Compiled libraries (.so files)
├── src/               # Source code
│   ├── core/          # Core program files
│   ├── displays/      # Graphics library implementations
│   ├── games/         # Game implementations
│   ├── interfaces/    # Interface definitions
│   ├── exceptions/    # Exception classes
│   └── utils/         # Utility classes
├── CMakeLists.txt     # CMake build configuration
├── Makefile           # Makefile for compilation
└── README.md          # Project overview
```

## Adding a New Graphics Library

To implement a new graphics library, follow these steps:

1. **Create a new class**: Create a new class that inherits from `ADisplayModule` in the `src/displays/` directory.

2. **Implement required methods**: Your class must implement all methods from the `IDisplayModule` interface:

```cpp
class YourGraphicsModule : public ADisplayModule {
public:
    YourGraphicsModule();
    ~YourGraphicsModule() override;
    
    void init() override;                   // Initialize graphics
    void stop() override;                   // Cleanup resources
    void clear() override;                  // Clear screen
    void display() override;                // Update display
    bool pollEvent(InputEvent &event) override; // Handle input events
    bool isOpen() const override;           // Check if window is open
    
    void drawRect(int x, int y, int width, int height, const Color &color) override;
    void drawCircle(int x, int y, int radius, const Color &color) override;
    void drawText(int x, int y, const std::string &text, const Color &color) override;
};
```

3. **Implement the entry point**: Your library must provide an entry point function:

```cpp
extern "C" {
    IDisplayModule *createDisplayModule() {
        return new YourGraphicsModule();
    }
}
```

4. **Update your build system**: Add your new display module to the CMakeLists.txt file.

## Adding a New Game

To implement a new game, follow these steps:

1. **Create a new class**: Create a new class that inherits from `AGameModule` in the `src/games/` directory.

2. **Implement required methods**: Your class must implement all methods from the `IGameModule` interface:

```cpp
class YourGame : public AGameModule {
public:
    YourGame();
    ~YourGame() override;
    
    void init() override;                // Initialize game
    void stop() override;                // Cleanup resources
    void update(float deltaTime) override; // Update game state
    void render(IDisplayModule &display) override; // Render game
    void handleEvent(const InputEvent &event) override; // Handle input
    void reset() override;               // Reset game state
};
```

3. **Implement the entry point**: Your library must provide an entry point function:

```cpp
extern "C" {
    IGameModule *createGameModule() {
        return new YourGame();
    }
}
```

4. **Update your build system**: Add your new game module to the CMakeLists.txt file.

## Building and Compiling

To build your new libraries, use the provided build system:

### Using CMake:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Using Makefile:
```bash
# Build everything
make

# Build just the core
make core

# Build just the graphics libraries
make graphicals

# Build just the games
make games
```

The compiled libraries will be placed in the `./lib/` directory.

## Interface Compatibility

To ensure compatibility with other Arcade implementations, follow these guidelines:

1. **Stick to the interfaces**: Don't modify the interfaces unless necessary.
2. **Use the same entry point**: Use the standard entry point function names.
3. **Handle errors gracefully**: Provide meaningful error messages.
4. **Follow the naming convention**: Use `arcade_<name>.so` for your libraries.

Remember that graphics libraries must not influence game logic, and game libraries must not contain information about screen rendering or low-level events.
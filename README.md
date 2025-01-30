# GrillingVNEngine

A lightweight Visual Novel engine built with C++ and SFML.

## Features

- YAML-based script system for easy story creation
- Dynamic character management with sprite support
- Background image handling with automatic scaling
- Music system with fade in/out effects
- Text animation and dialog system
- Multi-language support with UTF-8 encoding

## Upcoming Features

### Version 0.2.0
- Colored text support in dialogs using markup tags
- Automatic character movement activation
- Custom application icon support
- Game build tooling:
  - Resource package creation
  - Game distribution builder
  - Asset compression and optimization
  - One-click build system for final releases

## Getting Started

### Prerequisites

- CMake 3.28+
- C++17 compatible compiler
- SFML 3.0.0+
- yaml-cpp

### Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Script Structure

Scripts are written in YAML format. Example:

```yaml
scene_name: "Scene Title"
background: "path/to/background.png"
next_scene: "next_scene.yaml"
font: "path/to/font.ttf"

characters:
  - name: "Character1"
    sprites:
      default: "path/to/sprite.png"
      happy: "path/to/happy.png"
    initial_position: [400, 300]

music:
  - name: "bgm"
    path: "path/to/music.ogg"
    loop: true

script:
  - type: "dialog"
    character: "Character1"
    text: "Hello, world!"
    expression: "happy"

  - type: "move"
    character: "Character1"
    position: [500, 300]
    duration: 2.0
    smooth: true

  - type: "music"
    track: "bgm"
    volume: 80
    fade_in: 2.0
```

## Available Commands

### Dialog
Shows character dialogue with optional character name and expression:
```yaml
type: "dialog"
character: "Name"  # Optional
text: "Dialog text"
expression: "sprite_key"  # Optional
```

### Move
Moves a character to a new position:
```yaml
type: "move"
character: "Name"
position: [x, y]
smooth: true   # Optional, enables smooth movement
duration: 2.0  # Optional, seconds, for smooth movement
```

### Music
Controls background music:
```yaml
type: "music"
track: "track_name"
volume: 80       # 0-100
fade_in: 2.0     # Optional
fade_out: 2.0    # Optional
```

## Project Structure

- `src/` - Source files
- `assets/` - Game resources (images, music, fonts)
  - `scripts/` - Script files
  - `backgrounds/` - Background images
  - `characters/` - Character sprites
  - `music/` - BGM and sound effects
  - `resources/fonts` - Fonts

## License

This project is licensed under the MIT License - see the LICENSE file for details.

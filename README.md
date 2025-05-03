# Konkr Game in C++ 🎮

Made by **Antoine B & Nathan W**

## Project Presentation

This project is a C++ implementation of the [Konkr.io](http://konkr.io) game, created for the Advanced Programming course at [**Telecom Physique Strasbourg**](https://www.telecom-physique.fr/) engineering school. The game features a hexagonal grid-based strategy game with multiple units, buildings, and territory control mechanics.

---

## Summary

- **✨ How to use ?**
- **🎮 Game Features**
- **🗺️ Map Creation**
- **🌳 Project Structure**
- **⚙️ Technical Details**
- **🛠️ Features implemented (for corrector only)**

---

## ✨ How to use ?

### 1 ─ Dependencies Installation

For Ubuntu/Debian systems, install the required SDL2 libraries:

```bash
$ sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-gfx-dev libsdl2-ttf-dev
```

### 2 ─ Build the Project

Compile the project using the following command:

```bash
$ make
```

### 3 ─ Run the Game

Launch the game with default map:
```bash
$ ./konkr
```

Or specify a custom map (go to the Project structure section to see available maps):
```bash
$ ./konkr maps/1v1_close
```

---

## 🎮 Game Features

### Core Mechanics

- **Turn-based Strategy**: Players take turns managing their territory and units
- **Territory Control**: Expand your territory by moving units to adjacent hexes
- **Resource Management**: Manage coins and territory upkeep
- **Unit Progression**: Upgrade units through a promotion system

### Units & Buildings

| Type | Description |
|------|-------------|
| Town | Main building, defines whether a player is alive or not |
| Castle | Defensive structure (strong)|
| Villager | Basic unit, can be upgraded |
| Pikeman | Medium-strength unit |
| Knight | Strong unit |
| Hero | Most powerful unit |

### Special Elements

- **Bandits**: Autonomous units that steal coins from territories
- **Bandit Camps**: Generate new bandits when enough coins are collected
- **Forests**: Natural obstacles
- **Treasures**: Collectible resources with random values spawned with a 1/4 chance each turn if no treasure is present on the map
- **Maybe even more if you dig enough !**

### Protection System

Units provide protection to adjacent territory based on their level:
- Higher-level units can block movement of lower-level enemy units
- Towns and Castles provide protection to surrounding territory

---

## 🗺️ Map Creation

Maps are created using two different ASCII maps with a specific format. The first map defines the territory colors, while the second map defines the entities and their positions :

### Territory Colors
| Character | Color |
|-----------|--------|
| r | Red |
| g | Green |
| b | Blue |
| y | Yellow |
| c | Cyan |
| m | Magenta |
| . | Default (soil) |

### Entity Placement
| Character | Entity | Description |
|-----------|---------|-------------|
| T | Town | Base building |
| C | Castle | Defensive structure |
| V | Villager | Basic unit |
| P | Pikeman | Medium unit |
| K | Knight | Strong unit |
| H | Hero | Elite unit |
| B | Bandit | Autonomous enemy |
| c | Bandit Camp | Bandit spawner |
| t | Treasure | Random value (1-10) |
| f | Forest | Obstacle |

---

## 🌳 Project Structure

```bash
< Project >
   |
   |-- core/                 # Core game mechanics
   |    |-- grid.cpp        # Hexagonal grid implementation
   |    |-- hex.cpp         # Hex coordinate system
   |
   |-- entities/            # Game entities
   |    |-- building.cpp    # Buildings implementation
   |    |-- entity.cpp      # Base entity class
   |    |-- entitymanager.cpp # Entity management
   |
   |-- game/               # Game logic
   |    |-- game.cpp       # Main game loop
   |    |-- rendergame.cpp # Rendering system
   |
   |-- players/            # Player management
   |    |-- player.cpp     # Player class
   |    |-- playermanager.cpp # Player systems
   |
   |-- ui/                 # User interface
   |    |-- button.cpp     # UI elements
   |    |-- data.cpp       # UI data handling
   |
   |-- maps/               # Game maps
   |    |-- 1v1_close     # Two player map, players close to each other
   |    |-- 2players      # Alternative two player map with a treasure to be chased for !
   |    |-- 4players      # Four player map
   |    |-- 6players      # Six player map
```

---

## ⚙️ Technical Details

### Modern C++ Features Used

- Smart Pointers (`std::shared_ptr` mostly)
- STL Containers and Algorithms (`std::vector`, `std::map`)
- Range-based for loops

_Code is compiled with -Wextra -Wall -Wpedantic -Werror flags to ensure high code quality._

### Graphics & UI

- SDL2 for rendering
- Custom hexagonal grid system
- Interactive UI with buttons and icons
- Territory coloring system
- Drag & drop unit movement

### Game Systems

- Path finding for territory connectivity (breadth-first search)
- Entity management system with buyable units
- Resource management with upkeep, income, and spending
- Turn-based game loop
- Smart handling of players removal to find the next player supposed to play
- Event handling system

---

## 🛠️ Features implemented (for corrector only)

| Feature | Status |
|---------|---------|
| Turn-based game | ✅ |
| Multiplayer version | ✅ |
| Autonomous bandits | ✅ |
| Hexagonal grid | ✅ |
| ASCII file loading | ✅ |
| Faction colouring | ✅ |
| Path finding / flooding to check if territory is connected to its town | ✅ |
| Buildings and troops | ✅ |
| Power hierarchy | ✅ |
| Protection system | ✅ |
| Troop fusion for promotion | ✅ |
| Income and salary payment | ✅ |
| Mutiny for unpaid troops | ✅ |
| Selection by click and keyboard | ✅ |
| Basic troop animations | ✅ |
| Drag & drop | ✅ |
| undo option (basic, just cancels all of what happened in the current player turn) | ✅ |
| Map editor for creating, modifying, and saving maps | ❌ |

## 🎨 Credits

- Original game concept: [Konkr.io](http://konkr.io)
- Font: OpenSans.ttf
- SDL2 and related libraries

---

# Thanks for playing! 🎮
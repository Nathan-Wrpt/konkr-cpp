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
- **🛠️ Features implemented table (TLDR for corrector)**

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

Launch the game with default map (1v1):
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
| Hero | Elite unit |

### Special Elements

- **Bandits**: Autonomous units that steal coins from territories
- **Bandit Camps**: Generate new bandits when enough coins are collected (5 coins)
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

| Character | Color          |
| :-------- | :------------- |
| r         | Red            |
| g         | Green          |
| b         | Blue           |
| y         | Yellow         |
| .         | Default (soil) |
| k         | Black          |
| c         | Cyan           |
| m         | Magenta        |
| o         | Orange         |
| p         | Pink           |
| l         | Lavender       |
| n         | Teal           |
| t         | Turquoise      |
| a         | Transparent    |

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
| . | Empty | No entity |

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

### Game Systems Explanation

We've chosen to implement the grid with the pointy-top orientation.
The entities are managed with a system of inheritance, where each unit type inherits from a base entity class.
The messiest part of the code is surely the event handler, handling all the interactions of the player (click, button pressed, etc.) and the game loop. If the project code was to be improved again, we would probably try to separate the event handler from the game loop to make it cleaner.
We tried to separate the code as much as we could by creating managers for entities, players, bandits etc. in order not to have a huge game.cpp file with everything in it (even though it is still quite big).

The path finding is kind of simple with a breadth-first search algorithm, and we check at the begining of each player turn if players territories are still connected to their town. If not, the hex is lost by the player and units on them become bandits, pretty much like in the OG game, with the only difference that this check happens only at the beginning of turns, and not directly when a unit cuts land.

The undo system is straightforward, at the beginning of a player turn, the game state is saved, and if the player wants to undo, we just restore the game state to the one saved. We've not implemented a more advanced system like in the OG game because doing a rewind system would be unfair in a multiplayer game (and would probably be hard to code as well).

All of the rendering is done in rendergame.cpp, with functions called from game.cpp. We limited the frame rate to 240 FPS so that the animations of the units stay the same for every computer (if they can reach 240 FPS though). For the animations, we opted for a synced animation between the units, so that a first units gives the beat for the other units of the player. For the zooming in and out, we simply changed the size of the hexes when scrolling the mouse wheel. You can press space to reset camera position and speed any time.

The buying system is totally functional, buying the unit when clicked on and if the player has enough money, placed on a virtual hex until it is placed on a valid hex, so that it can be refunded if it's wrongly placed or dropped in the water.
The drag & drop system is a simple rendering "illusion", the unit is still on the hex it was placed on, but we render it at the mouse position while held by the player. 

QoL : We chose to highlight where the units can me moved at and display a swords icon on units that can be defeated by the held one. When a player has moved all of his units and cannot buy any units anymore, the "turn" button is highlighted to show that the player can end his turn without remorse.

---

## 🛠️ Features implemented table (TLDR for corrector)


| Feature | Status |
|---------|---------|
| Turn-based game | ✅ |
| Multiplayer version | ✅ |
| Autonomous bandits | ✅ |
| Hexagonal grid (pointy-top) | ✅ |
| ASCII file loading | ✅ |
| Faction colouring | ✅ |
| Path finding to check if territory is connected to its town | ✅ |
| Buildings and troops | ✅ |
| Power hierarchy | ✅ |
| Protection system (except the walls) | ✅ |
| Troop fusion for promotion | ✅ |
| Income and salary payment | ✅ |
| Mutiny for unpaid troops (only the ones that can't be paid, we've been kinder than thr original game) | ✅ |
| Selection by click and keyboard | ✅ |
| Basic troop animations | ✅ |
| Drag & drop | ✅ |
| Undo option (basic, just cancels all of what happened in the current player turn, hard to go further with a multiplayer game) | ✅ |
| Map editor for creating, modifying, and saving maps | ❌ |

## 🎨 Credits

- Original game concept: [Konkr.io](http://konkr.io)
- Font: OpenSans.ttf
- SDL2 and related libraries

---

# Thanks for playing! 🎮
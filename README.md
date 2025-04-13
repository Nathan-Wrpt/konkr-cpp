# konkr-cpp
konkr game in cpp

To install dependencies on Ubuntu :
```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-gfx-dev libsdl2-ttf-dev
```

To build the project, run the following command in the root directory of the project:
```bash
make
```

To run the project, execute the following command:
```bash
./konkr
```

You can load a map from a file by simply passing the path to the file as an argument. Example : 
```bash
./konkr maps/1v1_close
``` 

Here are the different characters that can be put in the entityMap. All of the players related entities gotta be place on an hex with a color, otherwise it will just not spawn.
| Character | Entity         | Description                                                       |
|-----------|----------------|-------------------------------------------------------------------|
| .         | None           | No entity is placed on the hex                                    |
| B         | Bandit         | Adds a Bandit to the hex                                          |
| c         | Bandit Camp    | Adds a Bandit Camp to the hex                                     |
| t         | Treasure       | Adds a Treasure to the hex (with a random value between 1 and 10)   |
| T         | Town           | Adds a Town to the hex (assigned to the owning player)            |
| V         | Villager       | Adds a Villager to the hex (assigned to the owning player)          |
| C         | Castle         | Adds a Castle to the hex (assigned to the owning player)            |
| P         | Pikeman        | Adds a Pikeman to the hex (assigned to the owning player)           |
| K         | Knight         | Adds a Knight to the hex (assigned to the owning player)            |
| H         | Hero           | Adds a Hero to the hex (assigned to the owning player)              |


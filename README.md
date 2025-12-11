# Decision Tree Game

This project builds a simple text-based decision game using a custom tree data structure.  
Each node in the tree represents part of the story, and each branch represents a choice the player can make.

The game reads from `story.txt`, loads the tree, and then lets the user play through it.

## How to Build

1. Make sure you have CMake installed.
2. Create a build folder:
mkdir build
cd build

markdown
Copy code
3. Generate the project:
cmake ..

markdown
Copy code
4. Build it:
make

markdown
Copy code
5. Run the game:
./DecisionTreeGame

markdown
Copy code

The first run will generate a **story.txt** if it doesn't exist.

## Files in this Repo

- `main.cpp` — instructor-provided game logic  
- `tree.h` — full implementation of the tree + nodes  
- `README.md` — this file  
- `developer_log.md` — your dev log  
- `CMakeLists.txt` — build script  
- `story.txt` — shows up after first run

## Notes
The tree supports:
- Multiple parents  
- Multiple children  
- Memory-safe deletion  
- Searching  
- Tree printing  

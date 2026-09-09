# AI In Games Repository
Repository for all assignments pertaining to UNR CS 481, AI in Games

## Assignments
All assignments will be listen in their respective folders. 
All folders should follow the same general format (until projects switch from raw C++ to Unity files,) that format being:
- build folder: This is where the build files from the CMake go, nothing of note in here (SHOULD NOT BE IN REPO, MAKE USER CREATE THEIR OWN BUILD FOLDER)
- src folder: This is where any and all code that I make will reside. For any grading purposes beyond compilation, this is where all relevant code will be housed
- CMakeLists.txt: Basic CMake file.

## Usage
In order to compile any non-Unity files: 
- Open the desired assignment in any IDE
- Run the following code in the terminal:
  ```
  git submodule --init --recursive
  mkdir build
  cd build
  cmake ..
  ./ <assignment cpp file name>

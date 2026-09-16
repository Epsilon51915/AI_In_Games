cmake_minimum_required(VERSION 3.18)
project(PacPlusPlus LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(../raylib-cpp raylib)

#include(../assets/includeable.cmake)

file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(PacPlusPlus src/PacPlusPlus.cpp)
target_link_libraries(PacPlusPlus PUBLIC raylib raylib_cpp raygui)
set_property(TARGET PacPlusPlus PROPERTY CXX_STANDARD 20)

configure_file(../Assets/pacmanTileMap.txt tilemaps/LevelOneTilemap.txt COPYONLY)
#configure_file(../assets/audio/luckyPickup.mp3 audio/luckyPickup.mp3 COPYONLY)
#configure_file(../assets/audio/walk.mp3 audio/walk.mp3 COPYONLY)
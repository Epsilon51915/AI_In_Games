cmake_minimum_required(VERSION 3.18)
project(8puzzle LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(../raylib-cpp raylib)

#include(../assets/includeable.cmake)

file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(8puzzle src/8puzzle.cpp)
target_link_libraries(8puzzle PUBLIC raylib raylib_cpp raygui)
set_property(TARGET 8puzzle PROPERTY CXX_STANDARD 20)

#configure_file(../assets/audio/itemPickup.mp3 audio/itemPickup.mp3 COPYONLY)
#configure_file(../assets/audio/luckyPickup.mp3 audio/luckyPickup.mp3 COPYONLY)
#configure_file(../assets/audio/walk.mp3 audio/walk.mp3 COPYONLY)
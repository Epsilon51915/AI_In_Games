# Install script for directory: /home/tyler/Coding/AI_In_Games/raylib-cpp/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/AudioDevice.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/AudioStream.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/AutomationEventList.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/BoundingBox.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Camera2D.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Camera3D.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Color.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/FileData.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/FileText.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Font.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Functions.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Gamepad.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Image.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Keyboard.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Material.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Matrix.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Mesh.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/MeshUnmanaged.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Model.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/ModelAnimation.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Mouse.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Music.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Ray.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/RayCollision.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/RaylibException.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/raylib-cpp-utils.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/raylib-cpp.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/raylib.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/raymath.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Rectangle.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/RenderTexture.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/ShaderUnmanaged.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Shader.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Sound.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Text.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Texture.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/TextureUnmanaged.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Touch.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Vector2.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Vector3.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Vector4.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/VrStereoConfig.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Wave.hpp"
    "/home/tyler/Coding/AI_In_Games/raylib-cpp/include/Window.hpp"
    )
endif()


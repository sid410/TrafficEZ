cmake_minimum_required(VERSION 3.22.1)

project(
  TrafficEZ
  VERSION 1.0.1
  LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/CMakeSettings.cmake)

# Need to have CURL installed locally
find_package(CURL REQUIRED)

# Need to have OpenCV installed locally If OpenCV is not in your environment
# variables, do: set(OpenCV_DIR /path/to/opencv/build)
find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
  message(
    FATAL_ERROR
      "OpenCV not found. Please make sure it's installed and set OpenCV_DIR if necessary."
  )
endif()

# Set the project name to be used globally
string(CONCAT PROJECT_FULLSTRING "${PROJECT_NAME}" "-${PROJECT_VERSION_MAJOR}"
              ".${PROJECT_VERSION_MINOR}" ".${PROJECT_VERSION_PATCH}")
set(PROJECT_NAME_VER ${PROJECT_FULLSTRING})

# Update automatically the version in version_config.h to not dirty commit
# history, I have gitignored version_config.h so every fresh clone, please build
# first then reload project to remove error. Note: To remove autoconf errors
# (files with *.in extensions), update your editor settings, for example in vs
# code: "files.associations": {"*.in": "cpp"}
set(VER_CONFIG_H_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/version_config.h)
configure_file(template/version_config.h.in ${VER_CONFIG_H_PATH} @ONLY)

# Configure the Doxyfile (update the template when making changes, not directly)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/template/Doxyfile.in
               ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile @ONLY)

# Change the build name only on Windows OS
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # For Windows: TrafficEZ-M.m.p-Win64.exe
  set(EXECUTABLE_NAME ${PROJECT_NAME_VER}-Win64.exe)
else()
  # Default to only TrafficEZ-M.m.p
  set(EXECUTABLE_NAME ${PROJECT_NAME_VER})
endif()

add_subdirectory(src)
add_subdirectory(util/yaml-cpp)
add_subdirectory(util/FPSHelper)
add_subdirectory(util/onnxruntime)
add_subdirectory(util/OrtApiWrapper)

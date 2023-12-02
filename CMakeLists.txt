cmake_minimum_required(VERSION 3.22.1)

project(
  TrafficEZ
  VERSION 0.0.0
  LANGUAGES C CXX)

# Need to have OpenCV installed locally If OpenCV is not in your environment
# variables, do: set(OpenCV_DIR /path/to/opencv/build)
find_package(OpenCV REQUIRED)

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
configure_file(include/version_config.h.in ${VER_CONFIG_H_PATH})

# Change the build name only on Windows OS
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # For Windows: TrafficEZ-M.m.p-Win64.exe
  set(EXECUTABLE_NAME ${PROJECT_NAME_VER}-Win64.exe)
else()
  # Default to only TrafficEZ-M.m.p
  set(EXECUTABLE_NAME ${PROJECT_NAME_VER})
endif()

add_subdirectory(src)

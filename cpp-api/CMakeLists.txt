cmake_minimum_required (VERSION 3.10)

project (
  CPP_API
  VERSION 0.0.1
  LANGUAGES CXX)

# C++ Version
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

# Private main build options
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # Sets -std=c++xx instead of -std=g++xx
  set (CMAKE_CXX_EXTENSIONS OFF)

  # Sets the executable location
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

  # Sets the build directory
  set (CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build")

  # This option is for possible IDE Usage
  set_property (GLOBAL PROPERTY USE_FOLDERS ON)


  # Stuff to build Tests comes here

  # Stuff to build docs via Doxygen comes here
endif ()



# This is the main source dir
add_subdirectory (src)

# Add the config subdirectory to be read elsewhere
include_directories (config)

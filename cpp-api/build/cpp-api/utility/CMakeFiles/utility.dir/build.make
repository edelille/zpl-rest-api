# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ellie/git/auxline/zpl-server/cpp-api

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ellie/git/auxline/zpl-server/cpp-api/build

# Include any dependencies generated for this target.
include cpp-api/utility/CMakeFiles/utility.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include cpp-api/utility/CMakeFiles/utility.dir/compiler_depend.make

# Include the progress variables for this target.
include cpp-api/utility/CMakeFiles/utility.dir/progress.make

# Include the compile flags for this target's objects.
include cpp-api/utility/CMakeFiles/utility.dir/flags.make

cpp-api/utility/CMakeFiles/utility.dir/utility.cpp.o: cpp-api/utility/CMakeFiles/utility.dir/flags.make
cpp-api/utility/CMakeFiles/utility.dir/utility.cpp.o: /home/ellie/git/auxline/zpl-server/cpp-api/cpp-api/utility/utility.cpp
cpp-api/utility/CMakeFiles/utility.dir/utility.cpp.o: cpp-api/utility/CMakeFiles/utility.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ellie/git/auxline/zpl-server/cpp-api/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object cpp-api/utility/CMakeFiles/utility.dir/utility.cpp.o"
	cd /home/ellie/git/auxline/zpl-server/cpp-api/build/cpp-api/utility && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT cpp-api/utility/CMakeFiles/utility.dir/utility.cpp.o -MF CMakeFiles/utility.dir/utility.cpp.o.d -o CMakeFiles/utility.dir/utility.cpp.o -c /home/ellie/git/auxline/zpl-server/cpp-api/cpp-api/utility/utility.cpp

cpp-api/utility/CMakeFiles/utility.dir/utility.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/utility.dir/utility.cpp.i"
	cd /home/ellie/git/auxline/zpl-server/cpp-api/build/cpp-api/utility && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ellie/git/auxline/zpl-server/cpp-api/cpp-api/utility/utility.cpp > CMakeFiles/utility.dir/utility.cpp.i

cpp-api/utility/CMakeFiles/utility.dir/utility.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/utility.dir/utility.cpp.s"
	cd /home/ellie/git/auxline/zpl-server/cpp-api/build/cpp-api/utility && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ellie/git/auxline/zpl-server/cpp-api/cpp-api/utility/utility.cpp -o CMakeFiles/utility.dir/utility.cpp.s

# Object files for target utility
utility_OBJECTS = \
"CMakeFiles/utility.dir/utility.cpp.o"

# External object files for target utility
utility_EXTERNAL_OBJECTS =

cpp-api/utility/libutility.so: cpp-api/utility/CMakeFiles/utility.dir/utility.cpp.o
cpp-api/utility/libutility.so: cpp-api/utility/CMakeFiles/utility.dir/build.make
cpp-api/utility/libutility.so: cpp-api/utility/CMakeFiles/utility.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/ellie/git/auxline/zpl-server/cpp-api/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libutility.so"
	cd /home/ellie/git/auxline/zpl-server/cpp-api/build/cpp-api/utility && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/utility.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
cpp-api/utility/CMakeFiles/utility.dir/build: cpp-api/utility/libutility.so
.PHONY : cpp-api/utility/CMakeFiles/utility.dir/build

cpp-api/utility/CMakeFiles/utility.dir/clean:
	cd /home/ellie/git/auxline/zpl-server/cpp-api/build/cpp-api/utility && $(CMAKE_COMMAND) -P CMakeFiles/utility.dir/cmake_clean.cmake
.PHONY : cpp-api/utility/CMakeFiles/utility.dir/clean

cpp-api/utility/CMakeFiles/utility.dir/depend:
	cd /home/ellie/git/auxline/zpl-server/cpp-api/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ellie/git/auxline/zpl-server/cpp-api /home/ellie/git/auxline/zpl-server/cpp-api/cpp-api/utility /home/ellie/git/auxline/zpl-server/cpp-api/build /home/ellie/git/auxline/zpl-server/cpp-api/build/cpp-api/utility /home/ellie/git/auxline/zpl-server/cpp-api/build/cpp-api/utility/CMakeFiles/utility.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : cpp-api/utility/CMakeFiles/utility.dir/depend

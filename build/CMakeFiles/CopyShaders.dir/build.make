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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.27.7/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.27.7/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jianghaochen/Desktop/cse169_animation

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jianghaochen/Desktop/cse169_animation/build

# Utility rule file for CopyShaders.

# Include any custom commands dependencies for this target.
include CMakeFiles/CopyShaders.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/CopyShaders.dir/progress.make

CMakeFiles/CopyShaders:
	/opt/homebrew/Cellar/cmake/3.27.7/bin/cmake -E copy_directory /Users/jianghaochen/Desktop/cse169_animation/shaders /Users/jianghaochen/Desktop/cse169_animation/build/shaders

CopyShaders: CMakeFiles/CopyShaders
CopyShaders: CMakeFiles/CopyShaders.dir/build.make
.PHONY : CopyShaders

# Rule to build all files generated by this target.
CMakeFiles/CopyShaders.dir/build: CopyShaders
.PHONY : CMakeFiles/CopyShaders.dir/build

CMakeFiles/CopyShaders.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CopyShaders.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CopyShaders.dir/clean

CMakeFiles/CopyShaders.dir/depend:
	cd /Users/jianghaochen/Desktop/cse169_animation/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jianghaochen/Desktop/cse169_animation /Users/jianghaochen/Desktop/cse169_animation /Users/jianghaochen/Desktop/cse169_animation/build /Users/jianghaochen/Desktop/cse169_animation/build /Users/jianghaochen/Desktop/cse169_animation/build/CMakeFiles/CopyShaders.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/CopyShaders.dir/depend


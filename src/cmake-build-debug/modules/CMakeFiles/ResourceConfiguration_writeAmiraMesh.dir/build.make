# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/top2021/Downloads/clion-2020.3.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/top2021/Downloads/clion-2020.3.2/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/top2021/SIVERT/sivert-vis-project/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug

# Utility rule file for ResourceConfiguration_writeAmiraMesh.

# Include the progress variables for this target.
include modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh.dir/progress.make

modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Copying resources for writeAmiraMesh to build directory"
	cd /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules && /home/top2021/Downloads/clion-2020.3.2/bin/cmake/linux/bin/cmake -E copy_directory /home/top2021/SIVERT/sivert-vis-project/src/modules/writeAmiraMesh/resources/ /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/share/openwalnut/modules/writeAmiraMesh/

ResourceConfiguration_writeAmiraMesh: modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh
ResourceConfiguration_writeAmiraMesh: modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh.dir/build.make

.PHONY : ResourceConfiguration_writeAmiraMesh

# Rule to build all files generated by this target.
modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh.dir/build: ResourceConfiguration_writeAmiraMesh

.PHONY : modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh.dir/build

modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh.dir/clean:
	cd /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules && $(CMAKE_COMMAND) -P CMakeFiles/ResourceConfiguration_writeAmiraMesh.dir/cmake_clean.cmake
.PHONY : modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh.dir/clean

modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh.dir/depend:
	cd /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/top2021/SIVERT/sivert-vis-project/src /home/top2021/SIVERT/sivert-vis-project/src/modules /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/CMakeFiles/ResourceConfiguration_writeAmiraMesh.dir/depend


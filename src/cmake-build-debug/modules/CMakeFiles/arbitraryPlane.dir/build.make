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

# Include any dependencies generated for this target.
include modules/CMakeFiles/arbitraryPlane.dir/depend.make

# Include the progress variables for this target.
include modules/CMakeFiles/arbitraryPlane.dir/progress.make

# Include the compile flags for this target's objects.
include modules/CMakeFiles/arbitraryPlane.dir/flags.make

modules/CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.o: modules/CMakeFiles/arbitraryPlane.dir/flags.make
modules/CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.o: ../modules/arbitraryPlane/WMArbitraryPlane.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object modules/CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.o"
	cd /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.o -c /home/top2021/SIVERT/sivert-vis-project/src/modules/arbitraryPlane/WMArbitraryPlane.cpp

modules/CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.i"
	cd /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/top2021/SIVERT/sivert-vis-project/src/modules/arbitraryPlane/WMArbitraryPlane.cpp > CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.i

modules/CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.s"
	cd /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/top2021/SIVERT/sivert-vis-project/src/modules/arbitraryPlane/WMArbitraryPlane.cpp -o CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.s

# Object files for target arbitraryPlane
arbitraryPlane_OBJECTS = \
"CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.o"

# External object files for target arbitraryPlane
arbitraryPlane_EXTERNAL_OBJECTS =

lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: modules/CMakeFiles/arbitraryPlane.dir/arbitraryPlane/WMArbitraryPlane.cpp.o
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: modules/CMakeFiles/arbitraryPlane.dir/build.make
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: lib/libopenwalnut.so.1.5.0
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libboost_program_options.so.1.71.0
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libboost_thread.so.1.71.0
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.71.0
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libboost_system.so.1.71.0
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libboost_regex.so.1.71.0
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libOpenGL.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libGLX.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libGLU.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgWidget.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgViewer.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgText.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgSim.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgGA.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgDB.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgUtil.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosg.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libOpenThreads.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libboost_atomic.so.1.71.0
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libOpenGL.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libGLX.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libGLU.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgWidget.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgViewer.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgText.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgSim.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgGA.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgDB.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosgUtil.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libosg.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: /usr/lib/x86_64-linux-gnu/libOpenThreads.so
lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0: modules/CMakeFiles/arbitraryPlane.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library ../lib/openwalnut/arbitraryPlane/libarbitraryPlane.so"
	cd /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/arbitraryPlane.dir/link.txt --verbose=$(VERBOSE)
	cd /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules && $(CMAKE_COMMAND) -E cmake_symlink_library ../lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0 ../lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1 ../lib/openwalnut/arbitraryPlane/libarbitraryPlane.so

lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1: lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0
	@$(CMAKE_COMMAND) -E touch_nocreate lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1

lib/openwalnut/arbitraryPlane/libarbitraryPlane.so: lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0
	@$(CMAKE_COMMAND) -E touch_nocreate lib/openwalnut/arbitraryPlane/libarbitraryPlane.so

# Rule to build all files generated by this target.
modules/CMakeFiles/arbitraryPlane.dir/build: lib/openwalnut/arbitraryPlane/libarbitraryPlane.so

.PHONY : modules/CMakeFiles/arbitraryPlane.dir/build

modules/CMakeFiles/arbitraryPlane.dir/clean:
	cd /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules && $(CMAKE_COMMAND) -P CMakeFiles/arbitraryPlane.dir/cmake_clean.cmake
.PHONY : modules/CMakeFiles/arbitraryPlane.dir/clean

modules/CMakeFiles/arbitraryPlane.dir/depend:
	cd /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/top2021/SIVERT/sivert-vis-project/src /home/top2021/SIVERT/sivert-vis-project/src/modules /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules /home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules/CMakeFiles/arbitraryPlane.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/CMakeFiles/arbitraryPlane.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/2.8.12/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/2.8.12/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/local/Cellar/cmake/2.8.12/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/chris/Development/cmsd2/ChrisOS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/chris/Development/cmsd2/ChrisOS

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/local/Cellar/cmake/2.8.12/bin/ccmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/local/Cellar/cmake/2.8.12/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/chris/Development/cmsd2/ChrisOS/CMakeFiles /Users/chris/Development/cmsd2/ChrisOS/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/chris/Development/cmsd2/ChrisOS/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named kernel

# Build rule for target.
kernel: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 kernel
.PHONY : kernel

# fast build rule for target.
kernel/fast:
	$(MAKE) -f kernel/CMakeFiles/kernel.dir/build.make kernel/CMakeFiles/kernel.dir/build
.PHONY : kernel/fast

#=============================================================================
# Target rules for targets named boot

# Build rule for target.
boot: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 boot
.PHONY : boot

# fast build rule for target.
boot/fast:
	$(MAKE) -f kernel/boot/CMakeFiles/boot.dir/build.make kernel/boot/CMakeFiles/boot.dir/build
.PHONY : boot/fast

#=============================================================================
# Target rules for targets named mm

# Build rule for target.
mm: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 mm
.PHONY : mm

# fast build rule for target.
mm/fast:
	$(MAKE) -f kernel/mm/CMakeFiles/mm.dir/build.make kernel/mm/CMakeFiles/mm.dir/build
.PHONY : mm/fast

#=============================================================================
# Target rules for targets named terminal

# Build rule for target.
terminal: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 terminal
.PHONY : terminal

# fast build rule for target.
terminal/fast:
	$(MAKE) -f kernel/terminal/CMakeFiles/terminal.dir/build.make kernel/terminal/CMakeFiles/terminal.dir/build
.PHONY : terminal/fast

#=============================================================================
# Target rules for targets named utils

# Build rule for target.
utils: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 utils
.PHONY : utils

# fast build rule for target.
utils/fast:
	$(MAKE) -f kernel/utils/CMakeFiles/utils.dir/build.make kernel/utils/CMakeFiles/utils.dir/build
.PHONY : utils/fast

#=============================================================================
# Target rules for targets named kern

# Build rule for target.
kern: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 kern
.PHONY : kern

# fast build rule for target.
kern/fast:
	$(MAKE) -f kernel/kern/CMakeFiles/kern.dir/build.make kernel/kern/CMakeFiles/kern.dir/build
.PHONY : kern/fast

#=============================================================================
# Target rules for targets named ia32

# Build rule for target.
ia32: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 ia32
.PHONY : ia32

# fast build rule for target.
ia32/fast:
	$(MAKE) -f kernel/ia32/CMakeFiles/ia32.dir/build.make kernel/ia32/CMakeFiles/ia32.dir/build
.PHONY : ia32/fast

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... kernel"
	@echo "... boot"
	@echo "... mm"
	@echo "... terminal"
	@echo "... utils"
	@echo "... kern"
	@echo "... ia32"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system


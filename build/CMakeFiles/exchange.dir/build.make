# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.28.1/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.28.1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ratikiyer/Programming/quant/hft-exchange

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ratikiyer/Programming/quant/hft-exchange/build

# Include any dependencies generated for this target.
include CMakeFiles/exchange.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/exchange.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/exchange.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/exchange.dir/flags.make

CMakeFiles/exchange.dir/src/main.cpp.o: CMakeFiles/exchange.dir/flags.make
CMakeFiles/exchange.dir/src/main.cpp.o: /Users/ratikiyer/Programming/quant/hft-exchange/src/main.cpp
CMakeFiles/exchange.dir/src/main.cpp.o: CMakeFiles/exchange.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/ratikiyer/Programming/quant/hft-exchange/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/exchange.dir/src/main.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/exchange.dir/src/main.cpp.o -MF CMakeFiles/exchange.dir/src/main.cpp.o.d -o CMakeFiles/exchange.dir/src/main.cpp.o -c /Users/ratikiyer/Programming/quant/hft-exchange/src/main.cpp

CMakeFiles/exchange.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/exchange.dir/src/main.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ratikiyer/Programming/quant/hft-exchange/src/main.cpp > CMakeFiles/exchange.dir/src/main.cpp.i

CMakeFiles/exchange.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/exchange.dir/src/main.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ratikiyer/Programming/quant/hft-exchange/src/main.cpp -o CMakeFiles/exchange.dir/src/main.cpp.s

CMakeFiles/exchange.dir/src/orderbook.cpp.o: CMakeFiles/exchange.dir/flags.make
CMakeFiles/exchange.dir/src/orderbook.cpp.o: /Users/ratikiyer/Programming/quant/hft-exchange/src/orderbook.cpp
CMakeFiles/exchange.dir/src/orderbook.cpp.o: CMakeFiles/exchange.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/ratikiyer/Programming/quant/hft-exchange/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/exchange.dir/src/orderbook.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/exchange.dir/src/orderbook.cpp.o -MF CMakeFiles/exchange.dir/src/orderbook.cpp.o.d -o CMakeFiles/exchange.dir/src/orderbook.cpp.o -c /Users/ratikiyer/Programming/quant/hft-exchange/src/orderbook.cpp

CMakeFiles/exchange.dir/src/orderbook.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/exchange.dir/src/orderbook.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ratikiyer/Programming/quant/hft-exchange/src/orderbook.cpp > CMakeFiles/exchange.dir/src/orderbook.cpp.i

CMakeFiles/exchange.dir/src/orderbook.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/exchange.dir/src/orderbook.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ratikiyer/Programming/quant/hft-exchange/src/orderbook.cpp -o CMakeFiles/exchange.dir/src/orderbook.cpp.s

# Object files for target exchange
exchange_OBJECTS = \
"CMakeFiles/exchange.dir/src/main.cpp.o" \
"CMakeFiles/exchange.dir/src/orderbook.cpp.o"

# External object files for target exchange
exchange_EXTERNAL_OBJECTS =

exchange: CMakeFiles/exchange.dir/src/main.cpp.o
exchange: CMakeFiles/exchange.dir/src/orderbook.cpp.o
exchange: CMakeFiles/exchange.dir/build.make
exchange: CMakeFiles/exchange.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/ratikiyer/Programming/quant/hft-exchange/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable exchange"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/exchange.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/exchange.dir/build: exchange
.PHONY : CMakeFiles/exchange.dir/build

CMakeFiles/exchange.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/exchange.dir/cmake_clean.cmake
.PHONY : CMakeFiles/exchange.dir/clean

CMakeFiles/exchange.dir/depend:
	cd /Users/ratikiyer/Programming/quant/hft-exchange/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ratikiyer/Programming/quant/hft-exchange /Users/ratikiyer/Programming/quant/hft-exchange /Users/ratikiyer/Programming/quant/hft-exchange/build /Users/ratikiyer/Programming/quant/hft-exchange/build /Users/ratikiyer/Programming/quant/hft-exchange/build/CMakeFiles/exchange.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/exchange.dir/depend


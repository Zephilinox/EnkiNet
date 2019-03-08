cmake_minimum_required (VERSION 3.8)

project(Tests)

add_executable(Tests
"main.cpp"

"Benchmark.hpp"
"PacketTest.hpp"
"RPCTest.hpp"
)

include(Settings)

set_target_properties(
	Tests PROPERTIES
	#This is visual studio specific, can't remember what it does
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	#the .exe goes in this folder
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Tests/${CONFIGURATION}"
	#not sure, library files don't seem to go here'
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Tests/${CONFIGURATION}"
)

target_link_libraries(Tests EnkiNet)

include_directories(${CMAKE_SOURCE_DIR}/src)
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (EnkiNet "main.cpp" "networking/Packet.hpp" "networking/Packet.cpp")

set_property(TARGET EnkiNet PROPERTY CXX_STANDARD 17)
get_property(CXX_STANDARD_ TARGET EnkiNet PROPERTY CXX_STANDARD)
message(STATUS "CXX_STANDARD " ${CXX_STANDARD_})

set_target_properties(
	EnkiNet PROPERTIES
	#This is visual studio specific, can't remember what it does
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	#the .exe goes in this folder
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CONFIGURATION}"
	#not sure, library files don't seem to go here'
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CONFIGURATION}"
)
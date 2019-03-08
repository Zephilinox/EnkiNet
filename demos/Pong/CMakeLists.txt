cmake_minimum_required (VERSION 3.8)

project(Pong)

add_executable(Pong
"main.cpp"

"src/Ball.hpp"
"src/Collision.hpp"
"src/Game.hpp"
"src/Paddle.hpp"
"src/PlayerText.hpp"

"src/Ball.cpp"
"src/Collision.cpp"
"src/Game.cpp"
"src/Paddle.cpp"
"src/PlayerText.cpp"
)

set_property(TARGET Pong PROPERTY CXX_STANDARD 17)
get_property(CXX_STANDARD_ TARGET Pong PROPERTY CXX_STANDARD)
message(STATUS "CXX_STANDARD is: " ${CXX_STANDARD_})

set_target_properties(
	Pong PROPERTIES
	#This is visual studio specific, can't remember what it does
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	#the .exe goes in this folder
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Pong/${CONFIGURATION}"
	#not sure, library files don't seem to go here'
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Pong/${CONFIGURATION}"
)

target_link_libraries(Pong EnkiNet)

include_directories(${CMAKE_SOURCE_DIR}/src "src")
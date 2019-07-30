cmake_minimum_required (VERSION 3.15)

project(Pong)

add_executable(Pong
"main.cpp"

"src/Ball.hpp"
"src/Ball.cpp"

"src/Collision.hpp"
"src/Collision.cpp"

"src/Game.hpp"
"src/Game.cpp"

"src/Paddle.hpp"
"src/Paddle.cpp"

"src/PlayerText.hpp"
"src/PlayerText.cpp"

"src/Score.hpp"
"src/Score.cpp"
)

include("../../cmake/Settings.cmake")

set_target_properties(
	Pong PROPERTIES
	#This is visual studio specific, can't remember what it does
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/demos/Pong/"
	#the .exe goes in this folder
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Pong/${CONFIGURATION}"
	#not sure, library files don't seem to go here'
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Pong/${CONFIGURATION}"
)

target_link_libraries(Pong EnkiNet)

include_directories(${CMAKE_SOURCE_DIR}/src "src")

#remove old resources
file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/bin/Pong/Debug/resources/")
file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/bin/Pong/Release/resources/")

#copy current ones
file(COPY "${CMAKE_SOURCE_DIR}/demos/Pong/resources/"
    DESTINATION "${CMAKE_SOURCE_DIR}/bin/Pong/Debug/resources/"
)

file(COPY "${CMAKE_SOURCE_DIR}/demos/Pong/resources/"
    DESTINATION "${CMAKE_SOURCE_DIR}/bin/Pong/Release/resources/"
)
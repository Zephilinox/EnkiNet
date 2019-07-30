cmake_minimum_required (VERSION 3.15)

project(Asteroids)

add_executable(Asteroids
"main.cpp"

"src/Managers/InputManager.hpp"
"src/Managers/InputManager.cpp"

"src/CustomData.hpp"

"src/Game.hpp"
"src/Game.cpp"

"src/Player.hpp"
"src/Player.cpp"

"src/Asteroid.hpp"
"src/Asteroid.cpp"

"src/Bullet.hpp"
"src/Bullet.cpp"

"src/CollisionManager.hpp"
"src/CollisionManager.cpp"

"src/PlayerText.hpp"
"src/PlayerText.cpp"
)

include("../../cmake/Settings.cmake")

set_target_properties(
	Asteroids PROPERTIES
	#This is visual studio specific, can't remember what it does
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/demos/Asteroids"
	#the .exe goes in this folder
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Asteroids/${CONFIGURATION}"
	#not sure, library files don't seem to go here'
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Asteroids/${CONFIGURATION}"
)

target_link_libraries(Asteroids EnkiNet)

target_include_directories(Asteroids PRIVATE ${CMAKE_SOURCE_DIR}/demos/Asteroids/src/)

#remove old resources
file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/bin/Asteroids/Debug/resources/")
file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/bin/Asteroids/Release/resources/")

#copy current ones
file(COPY "${CMAKE_SOURCE_DIR}/demos/Asteroids/resources/"
    DESTINATION "${CMAKE_SOURCE_DIR}/bin/Asteroids/Debug/resources/"
)

file(COPY "${CMAKE_SOURCE_DIR}/demos/Asteroids/resources/"
    DESTINATION "${CMAKE_SOURCE_DIR}/bin/Asteroids/Release/resources/"
)
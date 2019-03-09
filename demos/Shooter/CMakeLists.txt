cmake_minimum_required (VERSION 3.11)

project(Shooter)

add_executable(Shooter
"main.cpp"

"src/Game.hpp"
"src/Game.cpp"
)

include(Settings)

set_target_properties(
	Shooter PROPERTIES
	#This is visual studio specific, can't remember what it does
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/demos/Shooter"
	#the .exe goes in this folder
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Shooter/${CONFIGURATION}"
	#not sure, library files don't seem to go here'
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Shooter/${CONFIGURATION}"
)

target_link_libraries(Shooter EnkiNet)

include_directories(${CMAKE_SOURCE_DIR}/src)

#remove old resources
file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/bin/Shooter/Debug/resources/")
file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/bin/Shooter/Release/resources/")

#copy current ones
file(COPY "${CMAKE_SOURCE_DIR}/demos/Shooter/resources/"
    DESTINATION "${CMAKE_SOURCE_DIR}/bin/Shooter/Debug/resources/"
)

file(COPY "${CMAKE_SOURCE_DIR}/demos/Shooter/resources/"
    DESTINATION "${CMAKE_SOURCE_DIR}/bin/Shooter/Release/resources/"
)
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (EnkiNet
	"main.cpp"
	"Architecture/Networking/Packet.cpp"
	"Architecture/Timer.cpp"
	"Game/Game.cpp"
)

set_property(TARGET EnkiNet PROPERTY CXX_STANDARD 17)
get_property(CXX_STANDARD_ TARGET EnkiNet PROPERTY CXX_STANDARD)
message(STATUS "CXX_STANDARD is: " ${CXX_STANDARD_})

#https://stackoverflow.com/questions/2368811/how-to-set-warning-level-in-cmake
if (MSVC) 
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
	
	if (CMAKE_CXX_FLAGS MATCHES "/WX")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
	endif()
endif()

set_target_properties(
	EnkiNet PROPERTIES
	#This is visual studio specific, can't remember what it does
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	#the .exe goes in this folder
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CONFIGURATION}"
	#not sure, library files don't seem to go here'
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CONFIGURATION}"
)

target_link_libraries(${PROJECT_NAME}
	sfml-graphics
	spdlog
	enet
	ws2_32
	winmm
)
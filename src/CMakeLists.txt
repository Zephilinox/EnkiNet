cmake_minimum_required (VERSION 3.8)

project(src)

# Add source to this project's executable.
add_library (src
	"main.cpp"
	
	"Architecture/Networking/PacketTest.hpp"
	"Architecture/Networking/Packet.hpp"
	"Architecture/Networking/Packet.cpp"
	
	"Architecture/Networking/RPC.hpp"
	"Architecture/Networking/RPCTest.hpp"
	
	"Architecture/Networking/Benchmark.hpp"
	
	"Architecture/Networking/Networking/Client.hpp"
	"Architecture/Networking/Networking/ClientHost.hpp"
	"Architecture/Networking/Networking/ClientHost.cpp"
	"Architecture/Networking/Networking/ClientStandard.hpp"
	"Architecture/Networking/Networking/ClientStandard.cpp"
	"Architecture/Networking/Networking/Server.hpp"
	"Architecture/Networking/Networking/ServerHost.hpp"
	"Architecture/Networking/Networking/ServerHost.cpp"
	
	"Architecture/Managers/NetworkManager.cpp"
	"Architecture/Managers/NetworkManager.hpp"
	
	"Architecture/Signals/Connection.cpp"
	"Architecture/Signals/Connection.hpp"
	"Architecture/Signals/Disconnector.hpp"
	"Architecture/Signals/Signal.hpp"
	"Architecture/Signals/Slot.hpp"
	
	"Architecture/Timer.hpp"
	"Architecture/Timer.cpp"
	"Architecture/Scenegraph.hpp"
	"Architecture/Scenegraph.cpp"
	"Architecture/Entity.cpp"
	"Architecture/Entity.hpp"
	"Architecture/GameData.cpp"
	"Architecture/GameData.hpp"
)

set_property(TARGET src PROPERTY CXX_STANDARD 17)
get_property(CXX_STANDARD_ TARGET src PROPERTY CXX_STANDARD)
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
	src PROPERTIES
	#This is visual studio specific, can't remember what it does
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	#the .exe goes in this folder
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CONFIGURATION}"
	#not sure, library files don't seem to go here'
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CONFIGURATION}"
)

target_link_libraries(src
	sfml-graphics
	spdlog
	enet
	ws2_32
	winmm
)
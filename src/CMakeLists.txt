cmake_minimum_required (VERSION 3.8)

project(EnkiNet)

set(ENKI_HEADERS 
	"EnkiNet/Networking/Packet.hpp"
	"EnkiNet/Networking/RPC.hpp"
	"EnkiNet/Networking/Networking/Client.hpp"
	"EnkiNet/Networking/Networking/ClientHost.hpp"
	"EnkiNet/Networking/Networking/ClientStandard.hpp"
	"EnkiNet/Networking/Networking/Server.hpp"
	"EnkiNet/Networking/Networking/ServerHost.hpp"
	"EnkiNet/Managers/NetworkManager.hpp"
	"EnkiNet/Signals/Connection.hpp"
	"EnkiNet/Signals/Disconnector.hpp"
	"EnkiNet/Signals/Signal.hpp"
	"EnkiNet/Signals/Slot.hpp"
	"EnkiNet/Timer.hpp"
	"EnkiNet/Scenegraph.hpp"
	"EnkiNet/Entity.hpp"
	"EnkiNet/GameData.hpp"
)

add_library (EnkiNet
	${ENKI_HEADERS}
	
	"EnkiNet/Networking/Packet.cpp"
	
	"EnkiNet/Networking/Networking/ClientHost.cpp"
	"EnkiNet/Networking/Networking/ClientStandard.cpp"
	"EnkiNet/Networking/Networking/ServerHost.cpp"
	
	"EnkiNet/Managers/NetworkManager.cpp"
	
	"EnkiNet/Signals/Connection.cpp"
	
	"EnkiNet/Timer.cpp"
	"EnkiNet/Scenegraph.cpp"
	"EnkiNet/Entity.cpp"
	"EnkiNet/GameData.cpp"
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
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/EnkiNet/${CONFIGURATION}"
	#for shared libraries, should this be different to static?
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	#for static libraries
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	#for debug lib suffix
	DEBUG_POSTFIX "-d"
)

if (UNIX)
	set(UNIX_LIBS pthread)
endif()

target_link_libraries(EnkiNet
	sfml-graphics
	spdlog
	enet
	"${UNIX_LIBS}"
)

INSTALL(TARGETS EnkiNet
LIBRARY DESTINATION "${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
ARCHIVE DESTINATION "${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
)

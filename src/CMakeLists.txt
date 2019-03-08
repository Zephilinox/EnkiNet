cmake_minimum_required (VERSION 3.11)

project(EnkiNet)

set(EnkiNet_HEADERS 
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

set (EnkiNet_SRC
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

include(FilesystemSourceGroup)
SOURCE_GROUP_BY_FOLDER(EnkiNet)

add_library (EnkiNet
	${EnkiNet_HEADERS}
	${EnkiNet_SRC}
)

include(Settings)

set_target_properties(
	EnkiNet PROPERTIES
	#This is visual studio specific, can't remember what it does
	VS_DEBUGGER_WORKING_DIRECTORY		"${CMAKE_SOURCE_DIR}"
	#the .exe goes in this folder
	RUNTIME_OUTPUT_DIRECTORY			"${CMAKE_SOURCE_DIR}/bin/EnkiNet/${CONFIGURATION}"
	#for shared libraries, should this be different to static?
	LIBRARY_OUTPUT_DIRECTORY			"${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	LIBRARY_OUTPUT_DIRECTORY_DEBUG		"${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE	"${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	#for static libraries
	ARCHIVE_OUTPUT_DIRECTORY			"${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG		"${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE	"${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	#for debug lib suffix
	DEBUG_POSTFIX						"-d"
)

if (UNIX)
	set(UNIX_LIBS pthread)
endif()

target_link_libraries(EnkiNet
	sfml-graphics
	spdlog
	enet
	${UNIX_LIBS}
)

INSTALL(TARGETS EnkiNet
	LIBRARY DESTINATION "${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
	ARCHIVE DESTINATION "${CMAKE_SOURCE_DIR}/bin/EnkiNet/lib"
)
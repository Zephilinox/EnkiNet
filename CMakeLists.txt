#I don't actually know
cmake_minimum_required (VERSION 3.8)

#Includes library and demos and stuff
project ("EnkiNetProject")

set(SFML_STATIC_LIBRARIES TRUE)
	
if (WIN32)
	set(SFML_DIR "${CMAKE_SOURCE_DIR}/../SFML-2.5.1/lib/cmake/SFML")
endif()

message(STATUS "Make sure SFML is installed and SFML_DIR is set")
message(STATUS "SFML_DIR is : " ${SFML_DIR})

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

#any directories cmake needs to care about basically
add_subdirectory ("src")
#all the demo projects
add_subdirectory ("demos")
#the enkinet library tests
add_subdirectory("tests")
#exclude from all does something I'm pretty sure
add_subdirectory ("third_party" EXCLUDE_FROM_ALL)
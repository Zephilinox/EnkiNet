#I don't actually know
cmake_minimum_required (VERSION 3.8)

#Our actual project
project ("EnkiNet")

#any directories cmake needs to care about basically
add_subdirectory ("src")
#exclude from all does something I'm pretty sure
add_subdirectory ("third_party" EXCLUDE_FROM_ALL)
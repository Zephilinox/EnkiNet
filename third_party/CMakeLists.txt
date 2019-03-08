cmake_minimum_required (VERSION 3.8)

add_subdirectory ("doctest")
add_subdirectory ("enet")
add_subdirectory ("spdlog")

target_include_directories(EnkiNet PUBLIC "enetpp/")
target_include_directories(EnkiNet PUBLIC "enetpp/include/")
target_include_directories(EnkiNet PUBLIC "enetpp/include/enetpp/")
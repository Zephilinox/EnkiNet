# EnkiNet
My networking dissertation (2018/2019) for implementing a HLAPI integrated in to a 2D scenegraph framework.
Cool modern name and shit innit

[Trello](https://trello.com/b/vID8EWtE/enkinet)

# Make sure SFML is installed
If on windows, modify SFML_DIR in /CMakeLists.txt to point to the SFML install (not source code) (2.5.1, 32-bit)

If on linux, good luck

# Generate solution with CMake
````
cd build
cmake ..
````

if generating a visual studio solution, remember to set "EnkiNet" as the start up project.

# Build with CMake
````
cd build
cmake --build .
cd ../bin/debug/
.\EnkiNet.exe
````
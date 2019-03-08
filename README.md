# EnkiNet
My networking dissertation (2018/2019) for implementing a HLAPI integrated in to a 2D scenegraph framework.

[Trello](https://trello.com/b/vID8EWtE/enkinet)

Tested and builds on Windows using MSVC 15.2, or on Linux using either GCC 8.2 or Clang 8. (older versions might also work)

# Make sure SFML is installed
If on windows, modify SFML_DIR in /CMakeLists.txt to point to the SFML install (not source code) (2.5.1, 32-bit)
If on linux, make sure to install libsfml-dev

# Generate solution with CMake

If on linux, choose between GCC or Clang

````
export CC=/usr/bin/gcc
export CXX=/usr/bin/gxx
````

or

````
export CC=/usr/bin/clang-8
export CXX=/usr/bin/clang++-8
````

make an empty folder, in this case I've called it build, then:


````
cd build
cmake ..
````

if generating a visual studio solution, remember to set up the start-up project.

# Build with CMake
````
cd build
cmake --build .
````

# Running demos

Demos will be built automatically. You may need to find the appropriate resources used in the demo and move them to the executable location (I will look in to doing this automatically at some point)
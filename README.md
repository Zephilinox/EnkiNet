# EnkiNet
My networking dissertation (2018/2019) for implementing a HLAPI integrated in to a 2D scenegraph framework.

Tested and builds on Windows using MSVC 15.2, or on Linux using either GCC 8.2 or Clang 8. (older versions might also work)

## Video
https://www.youtube.com/watch?v=YaYw5OSId7s

## Instructions
### Make sure SFML is installed
If on windows, modify SFML_DIR in /CMakeLists.txt to point to the SFML install (not source code) (2.5.1, 32-bit)
If on linux, make sure to install libsfml-dev

### Generate solution with CMake

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

If CMake complains about not being able to find SFML when it's already installed, either run `cmake ..` again or make sure you've modified the SFML_DIR path and it's pointing to the correct SFML version (2.5.1, 32-bit)

### Build with CMake
````
cd build
cmake --build .
````

### Demos and Tests

`cd build`

then

`cmake -D DEMOS=ON ..`

or

`cmake -D TESTS=ON ..`

or

`cmake -D DEMOS=ON -D TESTS=ON ..`

### Logging

EnkiNet uses SPDLOG for logging. In order to disable or limit which levels of logging are outputted, as well as change the output location, you need to create a logger with the name "EnkiNet" and modify it accordingly.

In the sample below the logger is modified after being constructed by EnkiNet. EnkiNet will construct a logger if one doesn't already exist when the Scenegraph, RPCManager, or NetworkManager are constructed.

````
auto enki_logger = spdlog::get("EnkiNet");
enki_logger->set_level(spdlog::level::err);
````

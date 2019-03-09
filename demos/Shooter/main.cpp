//STD
#include <iostream>

//LIBS
#include <EnkiNet/Timer.hpp>

int main(int argc, char** argv)
{
	Timer timer;
	std::cout << timer.getElapsedTime() << "\n";
	return 0;
}
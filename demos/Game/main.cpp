//STD
#include <iostream>

#include "../../src/Architecture/Timer.hpp"

int main(int argc, char** argv)
{
	Timer timer;
	std::cout << "timer " << timer.getElapsedTime() << "\n";
	return 0;
}
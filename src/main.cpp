//STD
#include <string>
#include <cassert>
#include <iostream>

//SELF
#include "networking/Packet.hpp"

int main()
{
	Packet p;

	std::string s = "hey";
	p << s;
	std::string s2 = ".";
	p >> s2;

	std::cout << s << "\n" << s2 << "\n";

	return 0;
}
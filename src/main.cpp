//SELF
#include "networking/Packet.hpp"

int main()
{
	Packet p;
	p << 5;
	int i;
	p >> i;
	return i;
}
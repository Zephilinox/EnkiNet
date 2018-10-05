//STD
#include <string>
#include <cassert>
#include <iostream>

//SELF
#include "networking/Packet.hpp"

struct vector2
{
	float x;
	float y;

	friend Packet& operator <<(Packet& p, const vector2& v);
	friend Packet& operator >>(Packet& p, vector2& v);
};

Packet& operator <<(Packet& p, const vector2& v)
{
	return p << v.x << v.y;
}

Packet& operator >>(Packet& p, vector2& v)
{
	return p >> v.x >> v.y;
}

int main()
{
	Packet p;

	std::string s = "hey";
	p << s;
	std::string s2 = ".";
	p >> s2;

	std::cout << s << "\n" << s2 << "\n";

	vector2 v{ 1, 3 };
	p << v;
	vector2 v2{0, 0};
	p >> v2;

	std::cout << v.x << " " << v.y << "\n";
	std::cout << v2.x << " " << v2.y << "\n";

	return 0;
}
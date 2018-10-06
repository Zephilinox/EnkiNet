﻿//STD
#include <string>
#include <cassert>
#include <iostream>

//SELF
#include "networking/Packet.hpp"

struct vector2
{
	float x = 0;
	float y = 0;

	template <class PacketHeader>
	friend Packet<PacketHeader>& operator <<(Packet<PacketHeader>& p, const vector2& v);
	template <class PacketHeader>
	friend Packet<PacketHeader>& operator >>(Packet<PacketHeader>& p, vector2& v);
};

template <class PacketHeader>
Packet<PacketHeader>& operator <<(Packet<PacketHeader>& p, const vector2& v)
{
	return p << v.x << v.y;
}

template <class PacketHeader>
Packet<PacketHeader>& operator >>(Packet<PacketHeader>& p, vector2& v)
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

	std::cout << "\n";
	vector2 v{ 1, 3 };
	p << v;
	vector2 v2;
	p >> v2;

	std::cout << v.x << " " << v.y << "\n";
	std::cout << v2.x << " " << v2.y << "\n";

	std::vector<vector2> vecs{ {1, 1}, {2, 2} };
	p << vecs;
	std::vector<vector2> vecs2{ {}, {}};
	p >> vecs2;

	std::cout << "\n";
	for (auto vec : vecs)
	{
		std::cout << vec.x << " " << vec.y << "\n";
	}

	for (auto vec : vecs2)
	{
		std::cout << vec.x << " " << vec.y << "\n";
	}

	std::cout << "\n";
	std::array<bool, 10> bools = {true, true, false, true, true, true, true, true, true, true};
	p << bools;
	std::array<bool, 10> bools2;
	p >> bools2;

	for (auto b : bools)
	{
		std::cout << std::boolalpha << b << " ";
	}
	std::cout << "\n";

	for (auto b : bools2)
	{
		std::cout << std::boolalpha << b << " ";
	}
	std::cout << "\n";

	std::cout << p.get_header().type << "\n";

	std::cout << static_cast<int>(p.get_bytes().data()[0]) << "\n";

	p.set_header({ PacketHeaderEnki::RPC });

	std::cout << p.get_header().type << "\n";

	std::cout << static_cast<int>(p.get_bytes().data()[0]) << "\n";

	return 0;
}
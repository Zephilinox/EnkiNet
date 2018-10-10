//STD
#include <string>
#include <cassert>
#include <iostream>

//LIBS
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

//SELF
#include "networking/Packet.hpp"

struct vector2
{
	float x = 0;
	float y = 0;

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

TEST_CASE("Packet")
{
	Packet p;

	SUBCASE("Strings")
	{
		std::string s = "hey";
		p << s;
		std::string s2 = ".";
		p >> s2;
		REQUIRE(s2 == s);
	}

	SUBCASE("Custom Class")
	{
		vector2 v{ 1, 3 };
		p << v;
		vector2 v2;
		p >> v2;
		REQUIRE(v2.x == v.x);
		REQUIRE(v2.y == v.y);
	}

	SUBCASE("Vector")
	{
		std::vector<int> vecs{1, 2};
		p << vecs;
		std::vector<int> vecs2{};
		p >> vecs2;
		REQUIRE(vecs2 == vecs);
	}

	SUBCASE("Array")
	{
		std::array<bool, 10> bools = { true, true, false, true, true, true, true, true, true, true };
		p << bools;
		std::array<bool, 10> bools2;
		p >> bools2;
		REQUIRE(bools2 == bools);
	}

	SUBCASE("Header")
	{
		int oldtype = p.get_header().type;
		int oldtype_bytes = static_cast<int>(p.get_bytes().data()[0]);
		REQUIRE(static_cast<int>(oldtype) == oldtype_bytes);

		p.set_header({ PacketType::GLOBAL_RPC });

		int type = p.get_header().type;
		int type_bytes = static_cast<int>(p.get_bytes().data()[0]);
		REQUIRE(static_cast<int>(type) == type_bytes);
	}
}

int main(int argc, char** argv)
{
	doctest::Context context;
	context.setOption("abort-after", 5);
	context.applyCommandLine(argc, argv);
	context.setOption("no-breaks", true);
	int result = context.run();
	if (context.shouldExit())
	{
		return result;
	}



	while (true);

	return result;
}
//STD
#include <string>
#include <cassert>
#include <iostream>
#include <map>
#include <functional>
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
		CHECK(s2 == s);
	}

	SUBCASE("Custom Class")
	{
		vector2 v{ 1, 3 };
		p << v;
		vector2 v2;
		p >> v2;
		CHECK(v2.x == v.x);
		CHECK(v2.y == v.y);
	}

	SUBCASE("Vector")
	{
		std::vector<int> vecs{1, 2};
		p << vecs;
		std::vector<int> vecs2{};
		p >> vecs2;
		CHECK(vecs2 == vecs);
	}

	SUBCASE("Array")
	{
		std::array<bool, 10> bools = { true, true, false, true, true, true, true, true, true, true };
		p << bools;
		std::array<bool, 10> bools2;
		p >> bools2;
		CHECK(bools2 == bools);
	}

	SUBCASE("Header")
	{
		int oldtype = p.get_header().type;
		int oldtype_bytes = static_cast<int>(p.get_bytes().data()[0]);
		CHECK(static_cast<int>(oldtype) == oldtype_bytes);

		p.set_header({ PacketType::GLOBAL_RPC });

		int type = p.get_header().type;
		int type_bytes = static_cast<int>(p.get_bytes().data()[0]);
		CHECK(static_cast<int>(type) == type_bytes);
	}

	SUBCASE("Write Bits")
	{
		//todo: remove new packet to check for accurate byte boundary changes
		Packet p2;
		int num = 0b00001111;
		p2.write_bits(num, 8);
		int resulting_num = static_cast<int>(p2.get_bytes().data()[2]);
		CHECK(num == resulting_num);

		int num2 = 0b01001100;
		p2.write_bits(num2, 8);
		int resulting_num2 = static_cast<int>(p2.get_bytes().data()[3]);
		CHECK(num2 == resulting_num2);

		int num3 = 0b00001111;
		p2.write_bits(num3, 4);
		int resulting_num3 = static_cast<int>(p2.get_bytes().data()[4]);
		CHECK(0b00001111 == resulting_num3);

		int num4 = 0b11110000;
		p2.write_bits(num4, 4, 4);
		int resulting_num4 = static_cast<int>(p2.get_bytes().data()[4]);
		CHECK(0b11111111 == resulting_num4);

		p2.write_bits(num4, 4);
		REQUIRE_THROWS(p2.write_bits(num4, 5));
	}

	SUBCASE("Read Bits")
	{
		Packet p2;
		p2 << 0b00001111;
		int input = static_cast<int>(p2.get_bytes().data()[2]);
		int output = 0b00000000;
		p2.read_bits(output, 8);
		CHECK(input == output);

		p2 << 0b11110000;
		int input2 = static_cast<int>(p2.get_bytes().data()[3]);
		int output2 = 0b00000000;
		p2.read_bits(output2, 8);
		CHECK(input2 == output2);

		Packet p3;
		int num = 0b11111111;
		p3.write_bits(num, 8);
		int output3 = 0;
		int output4 = 0;
		p3.read_bits(output3, 4);
		p3.read_bits(output4, 4, 4);
		CHECK(output3 == 0b00001111);
		CHECK(output4 == 0b11110000);
		CHECK(output3 + output4 == num);

		p3.read_bits(num, 4);
		REQUIRE_THROWS(p3.read_bits(num, 5););
	}
}

/////
template <typename T> T read(Packet& p)
{
	T t;
	p >> t;
	return t;
}

std::map<std::string, std::function<void(Packet)>> functions;

template<typename R, typename... Args>
void RegisterRPC_(std::string name, void(f)(Args...))
{
	functions[name] = [f](Packet p)
	{
		f(read<Args>(p)...);
	};
}

template <class F>
void RegisterRPC(std::string name, F f)
{
	RegisterRPC_<F>(name, f);
}

//https://stackoverflow.com/questions/15904288/how-to-reverse-the-order-of-arguments-of-a-variadic-template-function?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa

template <typename T>
void rpcPacket(Packet& p, T x)
{
	p << x;
}

template <typename T, typename... Args>
void rpcPacket(Packet& p, T x, Args... args)
{
	rpcPacket(p, args...);
	p << x;
}

template <typename... Args>
Packet rpcPacket(Args... args)
{
	Packet p;
	rpcPacket(p, args...);
	return p;
}

template <typename Signature>
struct signature;

template <typename Return, typename... Parameters>
struct signature<Return(Parameters...)>
{
	void call()
	{
		std::cout << "Signature has " << sizeof...(Parameters) << " params\n";
	}
};

void one(int i, double d, float s, int ii)
{
	std::cout << "function one(" << i << ", " << d << ", " << s << ", " << ii << ");\n";
}

TEST_CASE("RPC")
{
	RegisterRPC("one", one);
	functions["one"](rpcPacket(1, 2.0, 3.0f, 4));
	signature<decltype(one)>().call();
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
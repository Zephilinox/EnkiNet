﻿//STD
#include <string>
#include <cassert>
#include <iostream>
#include <map>
#include <functional>
#include <any>
#include <type_traits>

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

template <typename F>
void register_rpc(std::string name, F* func)
{
	static_assert(std::is_void<rpc<F>::return_t>::value,
		"You can't register a function as an RPC if it doesn't return void");
	rpc<F> rpc;
	rpc.add(name, func);
	std::cout << "rpc " << name << " registered\n";
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

template <typename not_important>
struct rpc;

template <typename Return, typename... Parameters>
struct rpc<Return(Parameters...)>
{
	using return_t = Return;
	void add(std::string name, std::function<Return(Parameters...)> f)
	{
		if (functions.count(name))
		{
			return;
		}

		std::cout << "function '" << name << "' has " << sizeof...(Parameters) << " parameters\n";
		functions[name] = [f](Packet p)
		{
			f(read<Parameters>(p)...);
		};
	}

	template <typename... Args>
	constexpr static bool is_correct_args()
	{
		return std::is_same_v<std::tuple<Parameters...>, std::tuple<Args...>>;
	}

	constexpr static int size_of_args()
	{
		return get_size_of_args<Parameters...>();
	}

private:
	template <typename T>
	constexpr static T make()
	{
		return T();
	}

	template <typename T>
	constexpr static int size_of(T thing)
	{
		return sizeof(thing);
	}

	template <typename T, typename... Args>
	constexpr static int size_of(T thing, Args... things)
	{
		return sizeof(thing) + size_of(things...);
	}

	template <typename... Args>
	constexpr static int get_size_of_args()
	{
		return size_of(make<Args>()...);
	}
};

void receive_rpc(Packet p)
{
	std::string name;
	p >> name;
	std::cout << "received packet to call rpc " << name << "\n";
	functions[name](p);
}

template <typename F, typename... Args>
void call_rpc([[maybe_unused]] F* f, std::string name, Args... args)
{
	if (functions.count(name))
	{
		static_assert(rpc<F>::is_correct_args<Args...>(), "You tried to call this rpc with the incorrect number or type of parameters");
		Packet p;

		//fill packet with rpc information
		p << name;
		rpcPacket(p, args...);

		//simulate client received packet
		receive_rpc(p);
	}
}

void one(int i, double d, float s, int ii)
{
	std::cout << "called function one(" << i << ", " << d << ", " << s << ", " << ii << ");\n";
}

TEST_CASE("RPC")
{
	register_rpc("one", one);
	//todo: use this to check that the data sent in the packet matches up in size to the data required, in case sender is malicious
	//kinda hard to check that each type in the packet is within a valid set of values, as long as the size is correct that should be fine
	//rpc functions will need to make sure that the data being passed to them is correct
	std::cout << "byte size of combined args: " << rpc<decltype(one)>::size_of_args() << "\n";
	call_rpc(one, "one", 1, 2.0, 3.0f, 4);
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
//STD
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
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <enet/enet.h>

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
	SUBCASE("Strings")
	{
		Packet p;
		std::string s = "hey";
		p << s;
		std::string s2 = ".";
		p >> s2;
		CHECK(s2 == s);
	}

	SUBCASE("Custom Class")
	{
		Packet p;
		vector2 v{ 1, 3 };
		p << v;
		vector2 v2;
		p >> v2;
		CHECK(v2.x == v.x);
		CHECK(v2.y == v.y);
	}

	SUBCASE("Vector")
	{
		Packet p;
		std::vector<int> vecs{1, 2};
		p << vecs;
		std::vector<int> vecs2{};
		p >> vecs2;
		CHECK(vecs2 == vecs);
	}

	SUBCASE("Array")
	{
		Packet p;
		std::array<bool, 10> bools = { true, true, false, true, true, true, true, true, true, true };
		p << bools;
		std::array<bool, 10> bools2;
		p >> bools2;
		CHECK(bools2 == bools);
	}

	SUBCASE("Header")
	{
		Packet p;
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
	}

	SUBCASE("Write Bits Overflowing Bytes")
	{
		Packet p;
		try
		{
			int num = 0b000001111;
			p.write_bits(num, 4);
			p.write_bits(num, 4);

			p.write_bits(num, 6);
			p.write_bits(num, 4);

			p.write_bits(num, 6);
		}
		catch (...)
		{

		}

		CHECK(p.get_bytes().size() == sizeof(PacketHeader) + 3);
		CHECK(static_cast<int>(p.get_bytes()[sizeof(PacketHeader)]) == 0b11111111);
		CHECK(static_cast<int>(p.get_bytes()[sizeof(PacketHeader) + 1]) == 0b11001111);
		CHECK(static_cast<int>(p.get_bytes()[sizeof(PacketHeader) + 2]) == 0b00111111);
	}

	SUBCASE("Read Bits Overflowing Bytes")
	{
		Packet p;
		int num = 0;
		p << 1000000;
		p.read_bits(num, 32);
		CHECK(num == 1000000);
	}

	SUBCASE("Compressed Range")
	{
		Packet p;
		float f1 = 0.5f;
		p.write_compressed_float(f1, 0, 1, 0.01f);
		float f2;
		p.read_compressed_float(f2, 0, 1, 0.01f);
		CHECK(f1 == f2);

		//will fail due to not supporting > 8 bits on read/write
		/*Packet p2;
		float f3 = 5;
		p2.write_compressed_float(f3, -10, 10, 0.01f);
		float f4;
		p2.read_compressed_float(f4, -10, 10, 0.01f);
		CHECK(f3 == f4);

		Packet p3;
		float f5 = 1.0f / 60.0f;
		p3.write_compressed_float(f5, 0, 1, 0.001f);
		float f6;
		p3.read_compressed_float(f6, 0, 1, 0.001f);
		CHECK(f5 == f6);*/
	}

	SUBCASE("Real Test")
	{
		Packet p({ PacketType::ENTITY });
		std::array<float, 2> position = { 300, 400 };
		float rotation = 27.27f;
		bool isplayer = true;
		int isplayer_int = static_cast<int>(isplayer);

		p.write_compressed_float(position[0], 0, 1280, 0.01f);
		p.write_compressed_float(position[1], 0, 720, 0.01f);
		p.write_compressed_float(rotation, 0, 360, 0.01f);
		p.write_bits(isplayer_int, 1); //todo: write bits not just for ints

		std::array<float, 2> received_pos;
		float received_rot;
		int received_isplayer;
		bool actual_isplayer;

		p.read_compressed_float(received_pos[0], 0, 1280, 0.01f);
		p.read_compressed_float(received_pos[1], 0, 720, 0.01f);
		p.read_compressed_float(received_rot, 0, 360, 0.01f);
		p.read_bits(received_isplayer, 1);
		actual_isplayer = static_cast<bool>(received_isplayer);

		CHECK(p.get_bytes().size() < sizeof(PacketHeader) + sizeof(position) + sizeof(rotation) + sizeof(isplayer));
		std::cout << p.get_bytes().size() << " < " << sizeof(PacketHeader) + sizeof(position) + sizeof(rotation) + sizeof(isplayer) << ", yay bytes saved!\n";
		CHECK(position[0] == doctest::Approx(received_pos[0]));
		CHECK(position[1] == doctest::Approx(received_pos[1]));
		CHECK(rotation == received_rot);
		CHECK(isplayer == actual_isplayer);
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
std::map<std::string, int> function_arguments_byte_size;

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
		function_arguments_byte_size[name] = size_of_args();
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
	try
	{
		if (p.get_bytes().size() <= p.get_bytes_read())
		{
			std::cout << "Invalid RPC packet received due to being empty, ignoring\n";
			return;
		}

		std::string name;
		p >> name;

		if (!functions.count(name))
		{
			std::cout << "Invalid RPC packet received due to invalid name, ignoring\n";
			return;
		}

		//I was going to check bytes left in packet here so that it could match up with the bytes the function expects
		//however, I realsied that deserializing each thing could lead it to change byte size (e.g. encoding a bool as a bit), so it wouldn't work here
		//it would have to be done once the entire packet has been deserialized, but at the moment it doesn't work that way
		//it's still safe enough though, an exception will throw and it will get caught.

		std::cout << "received packet to call rpc " << name << "\n";
		functions[name](p);
	}
	catch (std::exception& e)
	{
		std::cout << "Invalid RPC packet received that threw an exception, ignoring\n";
	}
}

template <typename F, typename... Args>
void call_rpc([[maybe_unused]] F* f, std::string name, Args... args)
{
	if (functions.count(name))
	{
		std::cout << "safe call to rpc " << name << " with the values";
		((std::cout << " " << args), ...);
		std::cout << "\n";
		static_assert(rpc<F>::is_correct_args<Args...>(), "You tried to call this rpc with the incorrect number or type of parameters");
		Packet p;

		//fill packet with rpc information
		p << name;
		rpcPacket(p, args...);

		//simulate client received packet
		receive_rpc(p);
	}
}

template <typename... Args>
void call_rpc_unsafe(std::string name, Args... args)
{
	if (functions.count(name))
	{
		std::cout << "unsafe call to rpc " << name << " with the values";
		((std::cout << " " << args), ...);
		std::cout << "\n";
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
	std::cout << "byte size of combined args: " << rpc<decltype(one)>::size_of_args() << "\n";
	call_rpc(one, "one", 1, 2.0, 3.0f, 4);
	call_rpc_unsafe("one", 1.5, -2.0f, true, true);

	//let's try and send a fake rpc
	call_rpc_unsafe("two", true);
	//this checks the function is registered already, so it's fine

	//let's do it manually
	Packet p;
	p << std::string("two") << true;
	receive_rpc(p);
	//this makes the client throw an exception, so we need to capture that exception rather than crash the game, now fixed

	//now let's try a valid rpc, but one which has the wrong types
	Packet p2;
	p2 << std::string("one") << true << true << true << true;
	receive_rpc(p2);
	//this also causes an exception to be thrown and the client to crash, now fixed

	//Now let's try sending one big arg to a valid function expecting 4 smaller args
	Packet p3;
	p3 << std::string("one") << 50 << 50;
	receive_rpc(p3);
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

	auto console = spdlog::stdout_color_mt("console");
	console->info("Hi :)");
	while (true);

	return result;
}
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

//TESTS
#include "networking/PacketTest.hpp"

/////
template <typename T> T read(Packet& p)
{
	T t;
	p >> t;
	return t;
}

//move to static class?
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
	constexpr static bool matches_arguments()
	{
		return std::is_same_v<std::tuple<Parameters...>, std::tuple<Args...>>;
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

		std::cout << "received packet to call rpc " << name << "\n";
		functions[name](p);
	}
	catch (std::exception&)
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
		static_assert(rpc<F>::matches_arguments<Args...>(), "You tried to call this rpc with the incorrect number or type of parameters");
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

	return 0;
}
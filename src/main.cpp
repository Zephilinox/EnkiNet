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
#include "networking/RPC.hpp"

//TESTS
#include "networking/PacketTest.hpp"
#include "networking/RPCTest.hpp"

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
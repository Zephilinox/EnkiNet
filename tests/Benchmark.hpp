#pragma once

#include <EnkiNet/Timer.hpp>
#include <EnkiNet/Networking/RPC.hpp>

void zero()
{
	
}

void one([[maybe_unused]]int i)
{
}

void two([[maybe_unused]]int x, [[maybe_unused]]int y)
{
}

void three(int clientID, [[maybe_unused]]int channelID, std::string message)
{
	static volatile int i = 0;
	i = clientID;
}

void math(int x_vel, int y_vel)
{
	static float xpos = 0;
	static float ypos = 0;
	static float xvel = 0;
	static float yvel = 0;
	xvel += x_vel * (1 / 60.0f);
	yvel += y_vel * (1 / 60.0f);

	xpos += xvel;
	ypos += yvel;

	if (xpos > 100)
	{
		ypos = xpos;
		xpos = 0;
	}
}

void print_time(Timer& t, int count)
{
	auto time = t.getChronoElapsedTime();
	std::cout << count << " calls took " <<
		std::chrono::duration_cast<Timer::seconds_float>(time).count() << " seconds\n";
	std::cout << "each call took " <<
		std::chrono::duration_cast<Timer::nanoseconds_float>(time).count() / count << " nanoseconds\n";
}

void benchmark_entity(Timer&t, int count)
{
	RPCManager rpcm;
	ent e;

	t.restart();
	for (int i = 0; i < count; ++i)
	{
		e.do_thing(1, i);
	}

	std::cout << "###[Entity::do_thing]\n";
	print_time(t, count);
	std::cout << e.i << "\n";

	t.restart();
	for (int i = 0; i < count; ++i)
	{
		//rpcm.call(&ent::do_thing, "do_thing", &e, 1, i);
	}
	std::cout << "###[Entity::do_thing] RPC\n";
	print_time(t, count);
	std::cout << "\n";
}

template <typename F, typename... Args>
void benchmark(F* function, std::string name, Timer t, int count, Args... args)
{
	RPCManager rpcm;
	t.restart();
	for (int i = 0; i < count; ++i)
	{
		function(args...);
	}
	std::cout << "### [" << name << "]\n";
	print_time(t, count);

	t.restart();
	for (int i = 0; i < count; ++i)
	{
		rpcm.call(function, name, args...);
	}
	std::cout << "###[" << name << "] RPC\n";
	print_time(t, count);
	std::cout << "\n";
}

void benchmark()
{
	RPCManager rpcm;
	rpcm.add("zero", &zero);
	rpcm.add("one", &one);
	rpcm.add("two", &two);
	rpcm.add("three", &three);
	rpcm.add("math", &math);

	Timer timer;

#ifdef _DEBUG
	int count = 50'000;
#else
	int count = 5'000'000;
#endif

	std::cout << "\n";
	//benchmark(&zero, "zero", timer, count);
	benchmark(&one, "one", timer, count, count);
	benchmark(&two, "two", timer, count, count * 2, count * 2);
	std::string s = "hello";
	benchmark(&three, "three", timer, count, 0, 0, s);
	benchmark(&math, "math", timer, count, count, count);

	benchmark_entity(timer, count);
}
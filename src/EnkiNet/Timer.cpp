#include "Timer.hpp"

Timer::Timer() noexcept
{
	restart();
}

float Timer::getElapsedTime() const noexcept
{
	return getElapsedTime<seconds>();
}

Timer::nanoseconds Timer::getChronoElapsedTime() const noexcept
{
	return clock::now() - start_time;
}

void Timer::restart() noexcept
{
	start_time = clock::now();
}
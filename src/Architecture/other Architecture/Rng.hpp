#pragma once
#include <random>

class Rng
{
public:
	Rng();

	int getRandomInt(int lower_bound, int upper_bound);
	float getRandomFloat(float lower_bound, float upper_bound);
	void setSeed(int seed);

private:
	std::random_device rd;
	std::mt19937 mt;
	int last_seed;
};
#pragma once
#include <random>

namespace common
{
struct Random
{
	// The device first: the engine is seeded from it (members are made in this order)
	std::random_device _device;
	std::mt19937 _engine;

	Random();
	~Random();

	template<typename T>
	T Generate(const T & from, const T & to)
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			return std::uniform_real_distribution<T>(from, to)(_engine);
		}
		else
		{
			return std::uniform_int_distribution<T>(from, to)(_engine);
		}
	}

	bool Probability(const float num);
};
}

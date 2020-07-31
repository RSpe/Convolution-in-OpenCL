#pragma once

#include <vector>
#include <random>

class generate_data_filter
{
public:
	static const std::vector<double> generate(int seed, int size);
};


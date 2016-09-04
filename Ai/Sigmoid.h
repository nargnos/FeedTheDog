#pragma once
#include <numeric>
#include "Linear.h"
#include <cmath>
#include "Define.h"

inline FloatingPoint _Sigmoid(FloatingPoint x)
{
	return 1 / (1 + exp(x));
}

struct Sigmoid
{
	template<typename TInputArray, typename TWeightArray>
	static FloatingPoint Transform(
		const TInputArray& input,
		const TWeightArray& weight,
		FloatingPoint threshold)
	{
		using namespace std;
		assert(size(input) == size(weight));
		return _Sigmoid(-Linear::Transform(input, weight, threshold));
	}

};

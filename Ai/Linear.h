#pragma once
#include <algorithm>
#include <numeric>
#include <amp.h>
#include <concurrent_vector.h>
#include "Define.h"
struct Linear
{
	template<typename TInputArray, typename TWeightArray>
	static FloatingPoint Transform(
		const TInputArray& input,
		const TWeightArray& weight,
		FloatingPoint threshold)
	{
		using namespace std;
		assert(size(input) == size(weight));
		return inner_product(begin(input), end(input), begin(weight), -threshold);
	}
};

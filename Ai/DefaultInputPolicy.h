#pragma once
#include <vector>
#include "Define.h"

struct DefaultInputPolicy
{
	static constexpr size_t GetSize(size_t inputSize)
	{
		return inputSize;
	}
	static _STD vector<FloatingPoint>& GetInput(_STD vector<FloatingPoint>& input)
	{
		return input;
	}
};
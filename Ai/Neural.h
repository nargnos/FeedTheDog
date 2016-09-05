#pragma once
#include <array>
#include "Define.h"
template<size_t _WeightSize>
	class Neural
{
public:
	Neural() = default;

	~Neural() = default;
	size_t GetWeightSize() const restrict(cpu,amp)
	{
		return _WeightSize;
	}
	_STD array<FloatingPoint, _WeightSize>& GetWeights() restrict(cpu,amp)
	{
		return weights_;
	}
	FloatingPoint& GetThreshold() restrict(cpu,amp)
	{
		return threshold_;
	}
	_STD array<FloatingPoint, _WeightSize>& GetWeights() const restrict(cpu,amp)
	{
		return weights_;
	}
	FloatingPoint GetThreshold() const restrict(cpu,amp)
	{
		return threshold_;
	}
	
	static constexpr size_t WeightSize = _WeightSize;
protected:
	_STD array<FloatingPoint, _WeightSize> weights_;
	FloatingPoint threshold_;
};
#pragma once
#include <array>
#include "Define.h"
template<size_t _WeightSize>
class Neural
{
public:
	using TWeight = FloatingPoint[_WeightSize];
	Neural() = default;

	~Neural() = default;
	size_t GetWeightSize() const restrict(cpu, amp)
	{
		return _WeightSize;
	}
	TWeight& GetWeights() restrict(cpu, amp)
	{
		return weights_;
	}

	const TWeight& GetWeights() const restrict(cpu, amp)
	{
		return weights_;
	}

	FloatingPoint GetThreshold() const restrict(cpu, amp)
	{
		return threshold_;
	}
	FloatingPoint& GetThreshold() restrict(cpu, amp)
	{
		return threshold_;
	}
	static constexpr size_t WeightSize = _WeightSize;
protected:
	TWeight weights_;
	FloatingPoint threshold_;
};
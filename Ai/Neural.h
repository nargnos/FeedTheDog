#pragma once
#include <array>
#include "Define.h"

// ��Ҫ�޸����Ĵ洢�ṹ�Ͷ��룬�����ط�Ҫ�õ�
template<size_t TWeightSize>
class Neural
{
public:
	using TWeight = FloatingPoint[TWeightSize];
	Neural() = default;

	~Neural() = default;
	size_t GetWeightSize() const restrict(cpu, amp)
	{
		return TWeightSize;
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
	static constexpr size_t WeightSize = TWeightSize;
protected:
	TWeight weights_;
	FloatingPoint threshold_;
};
#pragma once
#include <amp_math.h>
#include "Define.h"

struct SigmoidActivation
{
	static inline FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		return 1 / (1 + MATH_NAMESPACE exp(-x));
	}
	// x的值是Sigmoid的计算结果
	static inline FloatingPoint DerivativeConvert(FloatingPoint x) restrict(amp)
	{
		return x - MATH_NAMESPACE pow(x, 2);
	}
};
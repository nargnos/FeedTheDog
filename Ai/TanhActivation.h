#pragma once
#include <amp_math.h>
#include "Define.h"

struct TanhActivation
{
	static inline FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		return MATH_NAMESPACE tanh(x);
	}
	// x是函数计算的结果
	static inline FloatingPoint DerivativeConvert(FloatingPoint x) restrict(amp)
	{
		return 1 - MATH_NAMESPACE pow(x, 2);		
	}
};
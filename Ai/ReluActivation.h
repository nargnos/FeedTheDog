#pragma once
#include <amp_math.h>
#include "Define.h"

struct ReluActivation
{
	static inline FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		return MATH_NAMESPACE fmax(0, x);
	}
	static inline FloatingPoint DerivativeConvert(FloatingPoint x) restrict(amp)
	{
		return x < 0 ? 0 : 1;
	}
};
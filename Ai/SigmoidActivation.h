#pragma once
#include <amp_math.h>
#include "Define.h"

struct SigmoidActivation
{
	
	constexpr static bool IsDerivativeNeedBeforeData = false;
	static inline FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		return 1 / (1 + MATH_NAMESPACE exp(-x));
	}

	static inline FloatingPoint DerivativeConvert(FloatingPoint input, FloatingPoint convert) restrict(amp)
	{
		return convert - MATH_NAMESPACE pow(convert, 2);
	}
};
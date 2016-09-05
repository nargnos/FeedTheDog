#pragma once
#include <amp_math.h>
#include "Define.h"

struct SigmoidActivation
{
	static FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		// sigmoid
		return 1 / (1 + MATH_NAMESPACE exp(-x));
	}
};
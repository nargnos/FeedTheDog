#pragma once
#include <amp_math.h>
#include "Define.h"

struct ReluActivation
{
	constexpr static bool IsDerivativeNeedBeforeData = true;
	static inline FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		return MATH_NAMESPACE fmax(FloatingPoint(0.25 * x), x);
	}
	// input指经过Convert转换前的，convert是转换后的
	static inline FloatingPoint DerivativeConvert(FloatingPoint input, FloatingPoint convert) restrict(amp)
	{
		return static_cast<FloatingPoint>(input < 0. ? 0.25 : 1.);
	}
};
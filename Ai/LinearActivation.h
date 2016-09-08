#pragma once
#include "Define.h"

struct LinearActivation
{
	
	constexpr static bool IsDerivativeNeedBeforeData = false;
	static inline FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		return x;
	}
	static inline FloatingPoint DerivativeConvert(FloatingPoint input, FloatingPoint convert) restrict(amp)
	{
		// y=x 的导数应该是 1x^0=1 吧？
		return 1.;
	}
};
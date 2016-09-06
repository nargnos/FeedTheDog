#pragma once
#include "Define.h"

struct LinearActivation
{
	static inline FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		return x;
	}
	static inline FloatingPoint DerivativeConvert(FloatingPoint x) restrict(amp)
	{
		// y=x 的导数应该是 1x^0=1 吧？
		return 1.;
	}
};
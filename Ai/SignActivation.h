#pragma once
#include "Define.h"

struct SignActivation
{
	static inline FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		return x >= 0 ? 1 : -1;
	}
};
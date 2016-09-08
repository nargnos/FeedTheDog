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
		// y=x �ĵ���Ӧ���� 1x^0=1 �ɣ�
		return 1.;
	}
};
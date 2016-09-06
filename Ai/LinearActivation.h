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
		// y=x �ĵ���Ӧ���� 1x^0=1 �ɣ�
		return 1.;
	}
};
#pragma once
#include "Define.h"

struct BinaryStepActivation
{	
	constexpr static bool IsDerivativeNeedBeforeData = false;
	static inline FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		return static_cast<FloatingPoint>(x >= 0 ? 1 : 0);
	}
	// ò�Ʋ��ɵ���
	static inline FloatingPoint DerivativeConvert(FloatingPoint input, FloatingPoint convert) restrict(amp)
	{
		return 0.;
	}
};
#pragma once
#include "Define.h"

struct BinaryStepActivation
{
	static inline FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
		return x >= 0 ? 1 : 0;
	}
	// ò�Ʋ��ɵ���
};
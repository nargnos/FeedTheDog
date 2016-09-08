#pragma once
#include <vector>
#include "Define.h"
__interface ISample
{
	const _STD vector<FloatingPoint>& GetDesire() const;
	const _STD vector<FloatingPoint>& GetInput() const;

};
#pragma once
#include <memory>
#include <vector>
#include "Define.h"
#include "INeuralProxy.h"
__interface ILayer
{
	size_t GetInputSize() const;
	size_t GetNeuralCount() const;
	_STD unique_ptr<INeuralProxy> GetNeural(size_t index);
	void Transform(const _STD vector<FloatingPoint>& input, _Out_ _STD vector<FloatingPoint>& output) const;
};


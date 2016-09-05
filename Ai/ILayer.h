#pragma once
#include <memory>
#include <vector>
#include "Define.h"
#include "INeuralProxy.h"
#include "ISample.h"
__interface ILayer
{
	size_t GetInputSize() const;
	size_t GetNeuralCount() const;
	FloatingPoint Train(const ISample& sample);
	_STD unique_ptr<INeuralProxy> GetNeural(int index);
	void Transform(const _STD vector<FloatingPoint>& input, _Out_ _STD vector<FloatingPoint>& output) const;
};


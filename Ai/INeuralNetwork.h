#pragma once
#include <vector>
#include "Define.h"
#include "ILayerBuilder.h"
#include "ILayer.h"
__interface INeuralNetwork
{
	size_t GetLayerCount() const;
	void AddLayer(const ILayerBuilder& builder);
	const ILayer& GetLayer(size_t index) const;
	ILayer& GetLayer(size_t index);
	_STD vector<FloatingPoint> Transform(const _STD vector<FloatingPoint>& input) const;	
	size_t GetInputSize() const;
	size_t GetOutputSize() const;
	_STD vector<_STD vector<FloatingPoint>> GetWeights() const;
};
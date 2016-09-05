#pragma once
#include <memory>
#include <vector>
#include <cassert>
#include "ILayer.h"
class NeuralNetwork
{
public:
	NeuralNetwork()
	{
	}
	~NeuralNetwork()
	{
	}
	size_t GetLayerCount() const
	{
		return layers_.size();
	}

	const ILayer& GetLayer(size_t index) const
	{
		assert(index >= 0 && index < GetLayerCount());
		return *layers_[index];
	}
	_STD vector<FloatingPoint> Transform(const _STD vector<FloatingPoint>& input) const
	{
		_STD vector<FloatingPoint> result(input);
		for (auto& ptr : layers_)
		{
			ptr->Transform(result, result);
		}
		return result;
	}
protected:
	_STD vector<_STD unique_ptr<ILayer>> layers_;
};


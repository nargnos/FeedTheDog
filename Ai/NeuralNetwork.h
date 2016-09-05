#pragma once
#include <memory>
#include <vector>
#include <cassert>
#include "ILayer.h"
#include "ILayerBuilder.h"

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
	void AddLayer(const ILayerBuilder& builder)
	{
		auto tmp = builder.Build();
		assert(IsCanAdd(tmp));
		layers_.push_back(_STD move(tmp));
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
		result.resize(layers_.back()->GetNeuralCount());
		return result;
	}
protected:
	bool IsCanAdd(_STD unique_ptr<ILayer>& layer) const
	{
		if (!layers_.empty())
		{
			return layers_.back()->GetNeuralCount() == layer->GetInputSize();
		}
		return true;
	}
	_STD vector<_STD unique_ptr<ILayer>> layers_;
};


#pragma once
#include <memory>
#include <vector>
#include <cassert>
#include "ILayer.h"
#include "ILayerBuilder.h"
#include "INeuralNetwork.h"

class NeuralNetwork:
	public INeuralNetwork
{
public:
	NeuralNetwork() = default;
	~NeuralNetwork() = default;

	virtual size_t GetLayerCount() const override
	{
		return layers_.size();
	}
	virtual void AddLayer(const ILayerBuilder& builder)override
	{
		auto tmp = builder.Build();
		assert(IsLayerCanAdd(tmp));
		layers_.push_back(_STD move(tmp));
	}

	virtual const ILayer& GetLayer(size_t index) const override
	{
		assert(index >= 0 && index < GetLayerCount());
		return *layers_[index];
	}
	virtual ILayer& GetLayer(size_t index) override
	{
		assert(index >= 0 && index < GetLayerCount());
		return *layers_[index];
	}
	virtual _STD vector<FloatingPoint> Transform(const _STD vector<FloatingPoint>& input) const override
	{
		_STD vector<FloatingPoint> result(input);
		for (auto& ptr : layers_)
		{
			ptr->Transform(result, result);
		}
		result.resize(layers_.back()->GetNeuralCount());
		return result;
	}
	virtual size_t GetInputSize() const override
	{
		return (*layers_.begin())->GetInputSize();
	}
	virtual size_t GetOutputSize() const override
	{
		return (*layers_.begin())->GetNeuralCount();
	}

	virtual _STD vector<_STD vector<FloatingPoint>> GetWeights() const override
	{
		_STD vector<_STD vector<FloatingPoint>> result;
		for (auto& val : layers_)
		{
			auto bgn = val->_GetRawData();
			auto end = bgn + val->GetNeuralDataSize() * val->GetNeuralCount();
			result.emplace_back(bgn, end);
		}
		return result;
	}
protected:
	bool IsLayerCanAdd(_STD unique_ptr<ILayer>& layer) const
	{
		if (!layers_.empty())
		{
			return layers_.back()->GetNeuralCount() == layer->GetInputSize();
		}
		return true;
	}
	_STD vector<_STD unique_ptr<ILayer>> layers_;

	
};


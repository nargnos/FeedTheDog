#include "stdafx.h"
#include "NeuralNetwork.h"

size_t NeuralNetwork::GetLayerCount() const
{
	return layers_.size();
}

void NeuralNetwork::AddLayer(const ILayerBuilder & builder)
{
	auto tmp = builder.Build();
	assert(IsLayerCanAdd(tmp));
	layers_.push_back(_STD move(tmp));
}

const ILayer & NeuralNetwork::GetLayer(size_t index) const
{
	assert(index >= 0 && index < GetLayerCount());
	return *layers_[index];
}

ILayer & NeuralNetwork::GetLayer(size_t index)
{
	assert(index >= 0 && index < GetLayerCount());
	return *layers_[index];
}

std::vector<FloatingPoint> NeuralNetwork::Transform(const std::vector<FloatingPoint>& input) const
{
	_STD vector<FloatingPoint> result(input);
	for (auto& ptr : layers_)
	{
		ptr->Transform(result, result);
	}
	result.resize(layers_.back()->GetNeuralCount());
	return result;
}

size_t NeuralNetwork::GetInputSize() const
{
	return (*layers_.begin())->GetInputSize();
}

size_t NeuralNetwork::GetOutputSize() const
{
	return (*layers_.begin())->GetNeuralCount();
}

std::vector<std::vector<FloatingPoint>> NeuralNetwork::GetWeights() const
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

bool NeuralNetwork::IsLayerCanAdd(std::unique_ptr<ILayer>& layer) const
{
	if (!layers_.empty())
	{
		return layers_.back()->GetNeuralCount() == layer->GetInputSize();
	}
	return true;
}

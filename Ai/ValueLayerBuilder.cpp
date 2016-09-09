#include "stdafx.h"
#include <ppl.h>
#include "ValueLayerBuilder.h"

ValueLayerBuilderBase::ValueLayerBuilderBase(const std::initializer_list<std::initializer_list<FloatingPoint>>& weights)
{
	using namespace std;
	for (auto& row : weights)
	{
		values_.emplace_back(row);
	}
}

ValueLayerBuilderBase::ValueLayerBuilderBase(const std::vector<FloatingPoint>& weights, size_t neuralDataSize, size_t neuralCount)
{
	for (size_t i = 0; i < neuralCount; i++)
	{
		auto bgn = weights.begin() + i * neuralDataSize;
		auto end = bgn + neuralDataSize;
		values_.emplace_back(bgn, end);
	}
}

void ValueLayerBuilderBase::FillValues(std::unique_ptr<ILayer>& des) const
{
	auto neuralCount = des->GetNeuralCount();
	concurrency::parallel_for(size_t(0), neuralCount, [&](size_t idx)
	{
		auto& val = values_[idx];
		des->SetThreshold(idx, val.back());
		auto weightSize = des->GetNeuralWeightSize();
		for (size_t i = 0; i < weightSize; i++)
		{
			des->SetWeight(idx, i, val[i]);
		}
	});
}

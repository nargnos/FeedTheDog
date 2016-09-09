#include "stdafx.h"
#include "RandomLayerBuilder.h"

RandomLayerBuilderBase::RandomLayerBuilderBase(FloatingPoint weightMin, FloatingPoint weightMax) :
	ng(_STD random_device()()),
	ud(weightMin, weightMax)
{
}

void RandomLayerBuilderBase::FillValues(std::unique_ptr<ILayer>& des) const
{
	auto neuralCount = des->GetNeuralCount();
	concurrency::parallel_for(size_t(0), neuralCount, [&](size_t idx)
	{
		des->SetThreshold(idx, GetRandomWeight());
		auto weightSize = des->GetNeuralWeightSize();
		for (size_t i = 0; i < weightSize; i++)
		{
			des->SetWeight(idx, i, GetRandomWeight());
		}
	});
}

FloatingPoint RandomLayerBuilderBase::GetRandomWeight() const
{
	return ud(ng);
}

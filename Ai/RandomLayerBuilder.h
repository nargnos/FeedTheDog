#pragma once
#include <random>
#include <ppl.h>
#include <type_traits>
#include "ILayerBuilder.h"

class RandomLayerBuilderBase :
	public ILayerBuilder
{
public:
	RandomLayerBuilderBase(FloatingPoint weightMin, FloatingPoint weightMax) :
		ng(seed()),
		ud(weightMin, weightMax)
	{
	}

	virtual ~RandomLayerBuilderBase() = default;

protected:
	void FillValues(_STD unique_ptr<ILayer>& des) const
	{
		auto neuralCount = des->GetNeuralCount();
		concurrency::parallel_for(size_t(0), neuralCount, [&](size_t idx)
		{
			auto tmp = des->GetNeural(idx);
			tmp->SetThreshold(GetRandomWeight());
			auto weightSize = tmp->GetWeightSize();
			for (size_t i = 0; i < weightSize; i++)
			{
				tmp->SetWeight(i, GetRandomWeight());
			}
		});
	}
	FloatingPoint GetRandomWeight() const
	{
		return ud(ng);
	}
private:
	_STD random_device seed;
	mutable _STD default_random_engine ng;
	_STD uniform_real_distribution<FloatingPoint> ud;
};


template<typename TLayer>
class RandomLayerBuilder :
	public RandomLayerBuilderBase
{
public:
	static_assert(_STD is_base_of<ILayer, TLayer>::value, "TLayer error");
	RandomLayerBuilder(FloatingPoint weightMin = -2.4 * TLayer::NeuralInputSize, FloatingPoint weightMax = 2.4 * TLayer::NeuralInputSize) :
		RandomLayerBuilderBase(weightMin, weightMax)
	{
	}

	virtual ~RandomLayerBuilder() = default;


	virtual _STD unique_ptr<ILayer> Build() const override
	{
		_STD unique_ptr<ILayer> result = _STD make_unique<TLayer>();
		FillValues(result);
		return result;
	}
protected:

};

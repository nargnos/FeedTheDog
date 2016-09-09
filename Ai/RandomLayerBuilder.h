#pragma once
#include <random>
#include <ppl.h>
#include <type_traits>
#include "ILayerBuilder.h"

class RandomLayerBuilderBase :
	public ILayerBuilder
{
public:
	RandomLayerBuilderBase(FloatingPoint weightMin, FloatingPoint weightMax);

	virtual ~RandomLayerBuilderBase() = default;

protected:
	void FillValues(_STD unique_ptr<ILayer>& des) const;
	FloatingPoint GetRandomWeight() const;
private:
	mutable _STD default_random_engine ng;
	_STD uniform_real_distribution<FloatingPoint> ud;
};


template<typename TLayer>
class RandomLayerBuilder :
	public RandomLayerBuilderBase
{
public:
	static_assert(_STD is_base_of<ILayer, TLayer>::value, "TLayer error");
	RandomLayerBuilder(FloatingPoint weightMin = -2.4 / TLayer::NeuralInputSize, FloatingPoint weightMax = 2.4 / TLayer::NeuralInputSize) :
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

#pragma once

#include <random>
#include <type_traits>
#include <vector>
#include "Define.h"
FloatingPoint _GenerateWeight(FloatingPoint defaultWeightMin, FloatingPoint defaultWeightMax)
{
	using namespace std;
	static random_device seed;
	static default_random_engine ng(seed());
	uniform_real_distribution<FloatingPoint> ud(defaultWeightMin, defaultWeightMax);
	return ud(ng);
}
template<typename TActivationFunction>
class Neuron
{
public:
	using TWeights = _STD vector<FloatingPoint>;

	explicit Neuron(int inputSize, FloatingPoint defaultWeightMin = -0.5, FloatingPoint defaultWeightMax = 0.5) :
		weights_(inputSize)
	{
		Generate(defaultWeightMin, defaultWeightMax);
	}

	Neuron(const TWeights& weights, FloatingPoint threshold) :
		weights_(weights),
		threshold_(threshold)
	{
	}

	virtual ~Neuron() = default;

	const TWeights& GetWeights() const
	{
		return weights_;
	}
	FloatingPoint GetThreshold() const
	{
		return threshold_;
	}
	template<typename TInputArray>
	FloatingPoint Transform(const TInputArray& input)
	{
		return TActivationFunction::Transform(input, weights_, threshold_);
	}
	int GetInputSize() const
	{
		return weights_.size();
	}

protected:
	void Generate(FloatingPoint defaultWeightMin, FloatingPoint defaultWeightMax)
	{
		using namespace std;
		threshold_ = _GenerateWeight(defaultWeightMin, defaultWeightMax);
		generate(begin(weights_), end(weights_), [=]()
		{
			return _GenerateWeight(defaultWeightMin, defaultWeightMax);
		});
	}

	TWeights weights_;
	FloatingPoint threshold_;
};

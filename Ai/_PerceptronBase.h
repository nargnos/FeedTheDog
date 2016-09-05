#pragma once
#include "Neuron.h"
#include "Define.h"
template<typename TActivationFunction>
class PerceptronBase :
	public Neuron<TActivationFunction>
{
public:
	using Neuron::Neuron;

	~PerceptronBase() = default;


	// 返回值为误差
	template<typename TInputArray, typename TDesire>
	FloatingPoint Train(const TInputArray& input, const TDesire& desire, FloatingPoint trainingRate = 0.5)
	{
		return desire.Train(input, weights_, Transform(input), trainingRate);
	}

	// func用来转换激活函数的计算结果
	template<typename TInputArray, typename TDesire, typename TFunc>
	FloatingPoint Train(const TInputArray& input, const TDesire& desire, TFunc&& func, FloatingPoint trainingRate = 0.5)
	{
		return desire.Train(input, weights_, func(Transform(input)), trainingRate);
	}

};
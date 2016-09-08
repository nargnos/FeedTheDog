#pragma once
#include <memory>
#include "ITrainer.h"
#include "INeuralNetwork.h"
#include "ISample.h"
class TrainerBase :
	public ITrainer
{
public:

	TrainerBase(_STD shared_ptr<INeuralNetwork>& network, FloatingPoint trainingRate, FloatingPoint momentum) :
		network_(network),
		trainingRate_(trainingRate),
		momentum_(momentum),
		delta_(network->GetLayerCount()),
		lastDeviation_(-1.)
	{

	}

	virtual ~TrainerBase() = default;
	virtual void AddSample(_STD shared_ptr<ISample>& sample) override
	{
		samples_.push_back(sample);
	}
protected:
	
	_STD shared_ptr<INeuralNetwork> network_;
	_STD vector<_STD shared_ptr<ISample>> samples_;
	_STD vector<_STD vector<FloatingPoint>> delta_;
	FloatingPoint trainingRate_;
	FloatingPoint lastDeviation_;
	FloatingPoint momentum_;
};


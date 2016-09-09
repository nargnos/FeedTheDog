#include "stdafx.h"
#include "TrainerBase.h"

TrainerBase::TrainerBase(std::shared_ptr<INeuralNetwork>& network, FloatingPoint trainingRate, FloatingPoint momentum) :
	network_(network),
	trainingRate_(trainingRate),
	momentum_(momentum),
	delta_(network->GetLayerCount()),
	lastDeviation_(-1.)
{

}

const std::vector<std::shared_ptr<ISample>>& TrainerBase::GetSamples() const
{
	return samples_;
}

void TrainerBase::AddSample(std::shared_ptr<ISample>& sample)
{
	samples_.push_back(sample);
}

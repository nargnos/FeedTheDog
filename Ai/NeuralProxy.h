#pragma once
#include "INeuralProxy.h"
template<typename TNeural>
class NeuralProxy :
	public INeuralProxy
{
public:

	NeuralProxy(TNeural& neural) :
		neural_(neural)
	{
	}

	virtual ~NeuralProxy() = default;

	virtual size_t GetWeightSize() const override
	{
		return neural_.GetWeightSize();
	}
	virtual FloatingPoint GetWeight(size_t index) const override
	{
		assert(index >= 0 && index < GetWeightSize());
		return neural_.GetWeights()[index];
	}
	virtual void SetWeight(size_t index, FloatingPoint value) override
	{
		assert(index >= 0 && index < GetWeightSize());
		neural_.GetWeights()[index] = value;
	}
	virtual FloatingPoint GetThreshold() const override
	{
		return neural_.GetThreshold();
	}
	virtual void SetThreshold(FloatingPoint value) override
	{
		neural_.GetThreshold() = value;
	}
protected:
	TNeural& neural_;

};


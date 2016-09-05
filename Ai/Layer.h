#pragma once
#include <array>
#include <cassert>
#include "ILayer.h"
#include "LayerBase.h"
#include "Neural.h"
#include "INeuralProxy.h"
#include "NeuralProxy.h"


template<size_t InputSize,
	size_t NeuralCount,
	typename TTransformPolicy,
	typename TTrainPolicy,
	typename TInputPolicy>
	class Layer :
	public LayerBase
{
public:
	constexpr size_t NeuralWeightSize = TInputPolicy::GetSize(InputSize);
	using NeuralType = Neural<NeuralWeightSize>;

	Layer() = default;
	~Layer() = default;

	virtual size_t GetInputSize() const override
	{
		return InputSize;
	}

	virtual size_t GetNeuralCount() const override
	{
		return NeuralCount;
	}

	virtual _STD unique_ptr<INeuralProxy> GetNeural(int index) override
	{
		assert(NeuralCount > index && index >= 0);
		return _STD make_unique<NeuralProxy>(_STD ref(neurals_[index]));
	}

	virtual void Transform(const _STD vector<FloatingPoint>& input, _Out_ _STD vector<FloatingPoint>& output) const override
	{
		assert(input.size() >= NeuralWeightSize);
		TTransformPolicy::Transform<NeuralType, NeuralCount>(input, output, neurals_);
	}
	virtual FloatingPoint Train(const ISample & sample) override
	{
		return TTrainPolicy::Train(sample, neurals_);
	}
protected:
	_STD array<NeuralType, NeuralCount> neurals_;
	
};
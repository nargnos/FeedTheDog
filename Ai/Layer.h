#pragma once
#include <array>
#include <cassert>
#include "ILayer.h"
#include "LayerBase.h"
#include "Neural.h"
#include "INeuralProxy.h"
#include "NeuralProxy.h"


template<size_t NeuralCount,
	size_t InputSize,
	typename TTransformPolicy,
	typename TTrainPolicy,
	typename TInputPolicy>
	class Layer :
	public LayerBase
{
public:
	static_assert(NeuralCount > 0, "NeuralCount > 0");
	constexpr static size_t NeuralInputSize = TInputPolicy::GetSize(InputSize);
	constexpr static size_t Count = NeuralCount;

	using NeuralType = Neural<NeuralInputSize>;

	Layer() :
		neurals_(NeuralCount)
	{
	}
	~Layer() = default;

	virtual size_t GetInputSize() const override
	{
		return InputSize;
	}

	virtual size_t GetNeuralCount() const override
	{
		return NeuralCount;
	}

	virtual _STD unique_ptr<INeuralProxy> GetNeural(size_t index) override
	{
		assert(NeuralCount > index && index >= 0);
		return _STD make_unique<NeuralProxy<NeuralType>>(_STD ref(neurals_[index]));
	}

	virtual void Transform(const _STD vector<FloatingPoint>& input, _Out_ _STD vector<FloatingPoint>& output) const override
	{
		assert(input.size() >= NeuralInputSize);
		TTransformPolicy::Transform<NeuralType>(input, output, neurals_);
	}
	virtual FloatingPoint Train(const ISample & sample) override
	{
		return TTrainPolicy::Train(sample, neurals_);
	}
protected:
	_STD vector<NeuralType> neurals_;
};



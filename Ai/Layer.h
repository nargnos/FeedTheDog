#pragma once
#include <array>
#include <cassert>
#include "ILayer.h"
#include "LayerBase.h"
#include "Neural.h"
#include "INeuralProxy.h"
#include "NeuralProxy.h"

// TInputSize为输入个数（可以被TInputPolicy过滤为实际传输给神经元的输入个数NeuralInputSize），
// TNeuralCount为输出个数
template<size_t TInputSize,
	size_t TNeuralCount,	
	typename TTransformPolicy,
	typename TTrainPolicy,
	typename TInputPolicy>
	class Layer :
	public LayerBase
{
public:
	static_assert(TNeuralCount > 0, "NeuralCount > 0");
	constexpr static size_t NeuralInputSize = TInputPolicy::GetSize(TInputSize);
	constexpr static size_t NeuralCount = TNeuralCount;

	using NeuralType = Neural<NeuralInputSize>;

	Layer() :
		neurals_(TNeuralCount)
	{
	}
	~Layer() = default;

	virtual size_t GetInputSize() const override
	{
		return TInputSize;
	}

	virtual size_t GetNeuralCount() const override
	{
		return TNeuralCount;
	}

	virtual _STD unique_ptr<INeuralProxy> GetNeural(size_t index) override
	{
		assert(TNeuralCount > index && index >= 0);
		return _STD make_unique<NeuralProxy<NeuralType>>(_STD ref(neurals_[index]));
	}

	virtual void Transform(const _STD vector<FloatingPoint>& input, _Out_ _STD vector<FloatingPoint>& output) const override
	{
		assert(input.size() >= NeuralInputSize);
		TTransformPolicy::Transform<NeuralType, TNeuralCount>(input, output, neurals_);
	}
	virtual FloatingPoint Train(const ISample & sample) override
	{
		return TTrainPolicy::Train(sample, neurals_);
	}
protected:
	_STD vector<NeuralType> neurals_;
};



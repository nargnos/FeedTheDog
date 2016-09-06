#pragma once
#include <initializer_list>
#include <cassert>
#include <type_traits>
#include <algorithm>
#include "ILayerBuilder.h"



class ValueLayerBuilderBase :
	public ILayerBuilder
{
public:
	ValueLayerBuilderBase(const _STD initializer_list<_STD initializer_list<FloatingPoint>> & weights)
	{
		using namespace std;
		for (auto& row : weights)
		{
			values_.emplace_back(row);
		}
	}

	virtual ~ValueLayerBuilderBase() = default;

protected:
	void FillValues(_STD unique_ptr<ILayer>& des) const
	{
		auto neuralCount = des->GetNeuralCount();
		concurrency::parallel_for(size_t(0), neuralCount, [&](size_t idx)
		{
			auto tmp = des->GetNeural(idx);
			auto& val = values_[idx];
			tmp->SetThreshold(val.back());
			auto weightSize = tmp->GetWeightSize();
			for (size_t i = 0; i < weightSize; i++)
			{
				tmp->SetWeight(i, val[i]);
			}
		});
	}
private:
	_STD vector<_STD vector<FloatingPoint>> values_;
};


template<typename TLayer>
class ValueLayerBuilder :
	public ValueLayerBuilderBase
{
public:
	static_assert(_STD is_base_of<ILayer, TLayer>::value, "TLayer error");

	// 输入格式为 { {权, ... , 阈值}, ... }
	ValueLayerBuilder(const _STD initializer_list<_STD initializer_list<FloatingPoint>> & weights) :
		ValueLayerBuilderBase(weights)
	{

	}

	virtual ~ValueLayerBuilder() = default;

	virtual _STD unique_ptr<ILayer> Build() const override
	{
		_STD unique_ptr<ILayer> result = _STD make_unique<TLayer>();
		FillValues(result);
		return result;
	}

};


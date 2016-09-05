#pragma once
#include <initializer_list>
#include <cassert>
#include <type_traits>
#include <algorithm>
#include "ILayerBuilder.h"

template<typename TLayer>
class ValueLayerBuilder :
	public ILayerBuilder
{
public:
	static_assert(_STD is_base_of<ILayer, TLayer>::value, "TLayer error");

	// { {È¨, ... , ãÐÖµ}, ... }
	ValueLayerBuilder(const _STD initializer_list<_STD initializer_list<FloatingPoint>> & weights)
	{
		using namespace std;
		for (auto& row:weights)
		{
			values_.emplace_back(row);
		}
	}

	virtual ~ValueLayerBuilder() = default;

	virtual _STD unique_ptr<ILayer> Build() const override
	{
		_STD unique_ptr<ILayer> result = _STD make_unique<TLayer>();
		auto neuralCount = result->GetNeuralCount();
		concurrency::parallel_for(size_t(0), neuralCount, [&](size_t idx)
		{
			auto tmp = result->GetNeural(idx);
			auto val = values_[idx];
			tmp->SetThreshold(val[TLayer::NeuralInputSize]);
			auto weightSize = tmp->GetWeightSize();
			for (size_t i = 0; i < weightSize; i++)
			{
				tmp->SetWeight(i, val[i]);
			}
		});
		return result;
	}
protected:
	_STD vector<_STD vector<FloatingPoint>> values_;
};


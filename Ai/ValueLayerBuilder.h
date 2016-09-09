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
	ValueLayerBuilderBase(const _STD initializer_list<_STD initializer_list<FloatingPoint>> & weights);
	ValueLayerBuilderBase(const _STD vector<FloatingPoint> & weights, size_t neuralDataSize, size_t neuralCount);
	virtual ~ValueLayerBuilderBase() = default;

protected:
	void FillValues(_STD unique_ptr<ILayer>& des) const;
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
	using ValueLayerBuilderBase::ValueLayerBuilderBase;

	virtual ~ValueLayerBuilder() = default;

	virtual _STD unique_ptr<ILayer> Build() const override
	{
		_STD unique_ptr<ILayer> result = _STD make_unique<TLayer>();
		FillValues(result);
		return result;
	}

};


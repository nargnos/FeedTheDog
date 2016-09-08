#pragma once
#include <initializer_list>
#include "ISample.h"
class Sample:
	public ISample
{
public:

	Sample(
		const _STD initializer_list<FloatingPoint>& input,		
		const _STD initializer_list<FloatingPoint>& desire):
		desire_(desire),
		input_(input)
	{
	}

	virtual ~Sample() = default;

	virtual const _STD vector<FloatingPoint>& GetDesire() const override
	{
		return desire_;
	}
	virtual const _STD vector<FloatingPoint>& GetInput() const override
	{
		return input_;
	}
protected:
	_STD vector<FloatingPoint> desire_;
	_STD vector<FloatingPoint> input_;
};


#pragma once
#include <cassert>
#include <algorithm>
#include "TrainerBase.h"
class Trainer :
	public TrainerBase
{
public:

	using TrainerBase::TrainerBase;

	virtual ~Trainer() = default;

	virtual FloatingPoint Train(bool isRandomShuffle = false) override;
protected:
	FloatingPoint Train(const ISample& sample);

};


#pragma once
#include <memory>
#include "Define.h"
#include "ISample.h"
__interface ITrainer
{
	void AddSample(_STD shared_ptr<ISample>& sample);
	FloatingPoint Train(bool isRandomShuffle);
	const _STD vector<_STD shared_ptr<ISample>>& GetSamples() const;
};
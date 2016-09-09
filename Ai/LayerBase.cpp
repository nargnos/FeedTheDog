#include "stdafx.h"
#include "LayerBase.h"

void LayerBase::_ResizeOutput(std::vector<FloatingPoint>& output, size_t neuralCount)
{
	if (output.size() < neuralCount)
	{
		output.resize(neuralCount);
	}
}

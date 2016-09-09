#pragma once
#include "Define.h"
#include "ILayer.h"
class LayerBase :
	public ILayer
{
public:

	LayerBase() = default;
	constexpr static FloatingPoint ThresholdInput = -1;
	virtual ~LayerBase() = default;
protected:
	static void _ResizeOutput(_STD vector<FloatingPoint> & output, size_t neuralCount);
};


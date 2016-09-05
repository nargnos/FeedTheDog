#pragma once
__interface INeuralProxy
{
	size_t GetWeightSize()const;
	FloatingPoint GetWeight(size_t index)const;
	void SetWeight(size_t index, FloatingPoint value);
	FloatingPoint GetThreshold() const;
	void SetThreshold(FloatingPoint value);
};
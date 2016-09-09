#include "stdafx.h"
#include "Trainer.h"

FloatingPoint Trainer::Train(bool isRandomShuffle)
{
	assert(!samples_.empty());

	FloatingPoint deviation = 0;

	auto sampleSize = samples_.size();


	if (isRandomShuffle)
	{
		_STD random_shuffle(_STD begin(samples_), _STD end(samples_));
	}
	for (auto& s : samples_)
	{
		deviation += Train(*s);
	}

	if (lastDeviation_ > 0)
	{
		// 调节学习速率
		if (deviation > lastDeviation_ * FloatingPoint(1.04))
		{
			trainingRate_ *= FloatingPoint(0.7);
		}
		else if (deviation < lastDeviation_)
		{
			trainingRate_ *= FloatingPoint(1.05);
		}
	}
	lastDeviation_ = deviation;
	return deviation;
}

FloatingPoint Trainer::Train(const ISample & sample)
{
	auto count = network_->GetLayerCount();
	_STD vector<_STD vector<FloatingPoint>> netWeight;
	netWeight.reserve(count);

	_STD vector<_STD vector<FloatingPoint>> output;
	output.reserve(count + 1);
	output.push_back(sample.GetInput());
	// 取得所有返回, i层返回下标对应 output[i+1]
	for (size_t i = 0; i < count; i++)
	{
		auto& layer = network_->GetLayer(i);
		output.emplace_back(layer.GetOutputSize());
		netWeight.emplace_back(layer.GetOutputSize());
		layer.GetNetWeightAndResult(output[i], netWeight[i], output[i + 1]);
	}
	// 输出层梯度计算
	_STD vector<_STD vector<FloatingPoint>> gradients(count);
	auto deviation = network_->GetLayer(count - 1).GetOutputLayerGradients(output.back(), netWeight.back(), sample.GetDesire(), gradients.back());
	// 隐藏层梯度计算
	for (int i = count - 2; i >= 0; i--)
	{
		auto idx = i + 1;
		network_->GetLayer(i).GetHiddenLayerGradients(output[idx], netWeight[i], gradients[idx], network_->GetLayer(idx), gradients[i]);
	}

	// 更新权值
	for (size_t i = 0; i < count; i++)
	{
		network_->GetLayer(i).UpdateWeight(output[i], gradients[i], delta_[i], trainingRate_, momentum_, delta_[i]);
	}
	return deviation;
}

#pragma once
#include <array>
#include <cassert>
#include <amp.h>
#include <ppl.h>
#include "ILayer.h"
#include "LayerBase.h"
#include "Neural.h"
#include "INeuralProxy.h"
#include "NeuralProxy.h"

// TInputSize为输入个数
// TNeuralCount为输出个数
template<size_t TInputSize,
	size_t TNeuralCount,
	typename TNeuralActivation>
	class Layer :
	public LayerBase
{
public:
	static_assert(TNeuralCount > 0, "NeuralCount > 0");

	// 神经元输入个数=权数
	constexpr static size_t NeuralInputSize = TInputSize;
	constexpr static size_t NeuralWeightSize = NeuralInputSize;

	constexpr static size_t NeuralCount = TNeuralCount;
	// 数据大小=权数+阈值个数
	constexpr static size_t NeuralDataSize = NeuralInputSize + 1;

	using NeuralType = Neural<NeuralWeightSize>;
	using TActivation = TNeuralActivation;

	Layer() :
		neurals_(TNeuralCount)
	{
	}
	~Layer() = default;
	// NeuralWeightSize = NeuralInputSize
	virtual size_t GetInputSize() const override
	{
		return NeuralInputSize;
	}

	virtual size_t GetNeuralCount() const override
	{
		return NeuralCount;
	}
	// NeuralWeightSize = NeuralInputSize
	virtual size_t GetNeuralWeightSize() const override
	{
		return NeuralWeightSize;
	}
	virtual size_t GetNeuralDataSize() const override
	{
		return NeuralDataSize;
	}
	virtual size_t GetOutputSize() const override
	{
		return NeuralCount;
	}

	virtual _STD unique_ptr<INeuralProxy> GetNeural(size_t index) override
	{
		assert(NeuralCount > index);
		return _STD make_unique<NeuralProxy<NeuralType>>(_STD ref(neurals_[index]));
	}

	virtual FloatingPoint GetWeight(size_t neuralIndex, size_t weightIndex) const override
	{
		assert(NeuralCount > neuralIndex);
		assert(NeuralWeightSize > weightIndex);

		return neurals_[neuralIndex].GetWeights()[weightIndex];
	}
	virtual FloatingPoint GetThreshold(size_t neuralIndex) const override
	{
		assert(NeuralCount > neuralIndex);

		return neurals_[neuralIndex].GetThreshold();
	}
	virtual void SetWeight(size_t neuralIndex, size_t weightIndex, FloatingPoint val) override
	{
		assert(NeuralCount > neuralIndex);
		assert(NeuralWeightSize > weightIndex);
		neurals_[neuralIndex].GetWeights()[weightIndex] = val;
	}

	virtual void SetThreshold(size_t neuralIndex, FloatingPoint val) override
	{
		assert(NeuralCount > neuralIndex);
		neurals_[neuralIndex].GetThreshold() = val;
	}
	virtual const FloatingPoint * _GetRawData() const override
	{
		assert(sizeof(NeuralType) == sizeof(FloatingPoint) * NeuralDataSize);
		return reinterpret_cast<const FloatingPoint *>(neurals_.data());
	}
	virtual void Transform(const _STD vector<FloatingPoint>& input, _Out_ _STD vector<FloatingPoint>& output) const override
	{
		assert(input.size() >= NeuralWeightSize);
		_ResizeOutput(output, NeuralCount);

		concurrency::array_view<const FloatingPoint, 1> inputView(NeuralWeightSize, input);
		concurrency::array_view<const NeuralType, 1> neuralsView(NeuralCount, neurals_);
		concurrency::array_view<FloatingPoint, 1> outputView(NeuralCount, output);
		outputView.discard_data();

		concurrency::extent<2> extent(NeuralCount, NeuralDataSize);

		concurrency::parallel_for_each(
			extent.tile<NeuralCount, NeuralDataSize>(),
			[=](concurrency::tiled_index<NeuralCount, NeuralDataSize> idx) restrict(amp)
		{
			auto neuralIndex = idx.global[0];
			auto weightIdx = idx.global[1];
			tile_static FloatingPoint tmpData[NeuralCount][NeuralDataSize];

			auto& tmpNeuralData = tmpData[neuralIndex];
			auto& tmpWeightData = tmpNeuralData[weightIdx];

			// 算 权重 * 输入
			if (weightIdx == NeuralWeightSize)
			{
				tmpWeightData = ThresholdInput * neuralsView[neuralIndex].GetThreshold();
			}
			else
			{
				auto tmp = neuralsView[neuralIndex].GetWeights()[weightIdx];
				tmp *= inputView[weightIdx];
				tmpWeightData = tmp;
			}

			idx.barrier.wait_with_tile_static_memory_fence();

			if (weightIdx == 0)
			{
				// 算 净权重并转化输出
				FloatingPoint netWeight = 0;
				for (size_t i = 0; i < NeuralDataSize; i++)
				{
					netWeight += tmpNeuralData[i];
				}

				// 净权重转换
				outputView[neuralIndex] = TActivation::Convert(netWeight);
			}

		});
		outputView.synchronize();

	}
	// FIX: 跟上面的有些冗余
	virtual void GetNetWeightAndResult(const _STD vector<FloatingPoint>& input, _Out_ _STD vector<FloatingPoint>& netWeight, _Out_ _STD vector<FloatingPoint>& output) const override
	{
		assert(input.size() >= NeuralWeightSize);
		_ResizeOutput(output, NeuralCount);
		_ResizeOutput(netWeight, NeuralCount);

		concurrency::array_view<const FloatingPoint, 1> inputView(NeuralWeightSize, input);
		concurrency::array_view<const NeuralType, 1> neuralsView(NeuralCount, neurals_);
		concurrency::array_view<FloatingPoint, 1> outputView(NeuralCount, output);
		outputView.discard_data();

		concurrency::array_view<FloatingPoint, 1> netWeightView(NeuralCount, netWeight);
		netWeightView.discard_data();

		concurrency::extent<2> extent(NeuralCount, NeuralDataSize);

		concurrency::parallel_for_each(
			extent.tile<NeuralCount, NeuralDataSize>(),
			[=](concurrency::tiled_index<NeuralCount, NeuralDataSize> idx) restrict(amp)
		{
			auto neuralIndex = idx.global[0];
			auto weightIdx = idx.global[1];
			tile_static FloatingPoint tmpData[NeuralCount][NeuralDataSize];

			auto& tmpNeuralData = tmpData[neuralIndex];
			auto& tmpWeightData = tmpNeuralData[weightIdx];

			// 算 权重 * 输入
			if (weightIdx == NeuralWeightSize)
			{
				tmpWeightData = ThresholdInput * neuralsView[neuralIndex].GetThreshold();
			}
			else
			{
				auto tmp = neuralsView[neuralIndex].GetWeights()[weightIdx];
				tmp *= inputView[weightIdx];
				tmpWeightData = tmp;
			}

			idx.barrier.wait_with_tile_static_memory_fence();

			if (weightIdx == 0)
			{
				// 算 净权重并转化输出
				FloatingPoint tmpNetWeight = 0;
				for (size_t i = 0; i < NeuralDataSize; i++)
				{
					tmpNetWeight += tmpNeuralData[i];
				}
				if (TActivation::IsDerivativeNeedBeforeData)
				{
					netWeightView[neuralIndex] = tmpNetWeight;
				}
				// 净权重转换
				outputView[neuralIndex] = TActivation::Convert(tmpNetWeight);
			}

		});
		outputView.synchronize();
		if (TActivation::IsDerivativeNeedBeforeData)
		{
			netWeightView.synchronize();
		}
	}

	// 当层为输出层时会用到
	virtual FloatingPoint GetOutputLayerGradients(
		const _STD vector<FloatingPoint>& lastOutput,
		const _STD vector<FloatingPoint>& netWeight, 
		const _STD vector<FloatingPoint>& desire,
		_Out_ _STD vector<FloatingPoint>& gradients) const override
	{
		assert(netWeight.size() == NeuralCount);
		assert(lastOutput.size() == NeuralCount);

		assert(desire.size() == NeuralCount);
		_ResizeOutput(gradients, NeuralCount);

		concurrency::array_view<const FloatingPoint, 1> lastOutputView(NeuralCount, lastOutput);
		concurrency::array_view<const FloatingPoint, 1> netWeightView(NeuralCount, netWeight);

		concurrency::array_view<const FloatingPoint, 1> desireView(NeuralCount, desire);
		concurrency::array_view<FloatingPoint, 1> gradientsView(NeuralCount, gradients);
		gradientsView.discard_data();

		_STD vector<FloatingPoint> deviations(NeuralCount);
		concurrency::array_view<FloatingPoint, 1> deviationView(NeuralCount, deviations);
		deviationView.discard_data();

		concurrency::parallel_for_each(
			gradientsView.extent,
			[=](concurrency::index<1> idx) restrict(amp)
		{
			auto lastOutputVal = lastOutputView[idx];
			auto netWeightVal = netWeightView[idx];

			auto deviation = desireView[idx] - lastOutputVal;
			gradientsView[idx] = TActivation::DerivativeConvert(netWeightVal, lastOutputVal) * deviation;
			// 记录误差平方
			deviationView[idx] = MATH_NAMESPACE pow(deviation, 2);
		});
		gradientsView.synchronize();
		deviationView.synchronize();
		return concurrency::parallel_reduce(_STD begin(deviations), _STD end(deviations), 0);
	}

	virtual void GetHiddenLayerGradients(
		const _STD vector<FloatingPoint>& lastOutput,
		const _STD vector<FloatingPoint>& netWeight, 
		const _STD vector<FloatingPoint>& nextLayerGradients,
		const ILayer & nextLayer,
		_Out_ _STD vector<FloatingPoint>& gradients) const override
	{
		auto nextLayerNeuralCount = nextLayer.GetNeuralCount();
		auto nextLayerDataSize = nextLayer.GetNeuralDataSize();
		auto nextLayerInputSize = nextLayer.GetInputSize();

		assert(nextLayerNeuralCount == nextLayerGradients.size());
		assert(netWeight.size() == NeuralCount);
		assert(nextLayerInputSize == NeuralCount);
		assert(lastOutput.size() == NeuralCount);

		_ResizeOutput(gradients, NeuralCount);

		concurrency::array_view<const FloatingPoint, 1> lastOutputView(NeuralCount, lastOutput);
		concurrency::array_view<const FloatingPoint, 1> netWeightView(NeuralCount, netWeight);
		concurrency::array_view<const FloatingPoint, 1> nextLayerGradientsView(nextLayerNeuralCount, nextLayerGradients);
		concurrency::array_view<const FloatingPoint, 2> nextLayerView(nextLayerNeuralCount, nextLayerDataSize, nextLayer._GetRawData());
		concurrency::array_view<FloatingPoint, 1> gradientsView(NeuralCount, gradients);
		gradientsView.discard_data();

		concurrency::parallel_for_each(
			gradientsView.extent,
			[=](concurrency::index<1> idx) restrict(amp)
		{
			FloatingPoint tmp(0);

			for (size_t i = 0; i < nextLayerNeuralCount; i++)
			{
				// 输出到的元的 梯度*权 之和
				tmp += nextLayerGradientsView[i] * nextLayerView(i, idx[0]);
			}
			auto netWeightVal = netWeightView[idx];
			auto lastOutputVal = lastOutputView[idx];
			gradientsView[idx] = TActivation::DerivativeConvert(netWeightVal, lastOutputVal) * tmp;
		});
		gradientsView.synchronize();
	}

	virtual void UpdateWeight(
		const _STD vector<FloatingPoint>& input,
		const _STD vector<FloatingPoint>& gradients,
		const _STD vector<FloatingPoint>& lastWeightDelta,
		FloatingPoint trainingRate,
		FloatingPoint momentum,
		_Out_ _STD vector<FloatingPoint>& weightDelta) override
	{
		assert(input.size() == NeuralInputSize);
		assert(gradients.size() == NeuralCount);
		_ResizeOutput(weightDelta, NeuralCount * NeuralDataSize);

		concurrency::array_view<const FloatingPoint, 1> inputView(NeuralInputSize, input);
		concurrency::array_view<const FloatingPoint, 1> gradientsView(NeuralCount, gradients);
		concurrency::array_view<FloatingPoint, 2> neuralsView(NeuralCount, NeuralDataSize, reinterpret_cast<FloatingPoint*>(neurals_.data()));
		concurrency::array_view<const FloatingPoint, 2> lastWeightDeltaView(NeuralCount, NeuralDataSize, lastWeightDelta);

		concurrency::array_view<FloatingPoint, 2> weightDeltaView(NeuralCount, NeuralDataSize, weightDelta);
		weightDeltaView.discard_data();

		concurrency::parallel_for_each(
			weightDeltaView.extent,
			[=](concurrency::index<2> idx) restrict(amp)
		{
			auto neuralIndex = idx[0];
			auto dataIndex = idx[1];
			FloatingPoint inputVal = dataIndex == NeuralInputSize ? ThresholdInput : inputView[dataIndex];

			auto delta = gradientsView[neuralIndex] * inputVal * trainingRate + momentum * lastWeightDeltaView[idx];
			weightDeltaView[idx] = delta;
			neuralsView[idx] += delta;
		});
		weightDeltaView.synchronize();
		neuralsView.synchronize();
	}

protected:
	_STD vector<NeuralType> neurals_;


};



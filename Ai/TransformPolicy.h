#pragma once
#include <vector>
#include <cassert>
#include <amp.h>
#include "Define.h"


struct TransformPolicyBase
{
	static void _ResizeOutput(_STD vector<FloatingPoint> & output, size_t neuralCount)
	{
		if (output.size() < neuralCount)
		{
			output.resize(neuralCount);
		}
	}
};
// FIX: 这里会导致不同参数的每一层都会编译不同的代码，需要把共性抽出来
template<typename TNeuralActivationFunction>
struct TransformPolicy :
	public TransformPolicyBase
{
	template<typename TNeuralType, size_t TNeuralCount, size_t TNeuralDataSize = TNeuralType::WeightSize + 1>
	static void Transform(
		const _STD vector<FloatingPoint>& input,
		_Out_ _STD vector<FloatingPoint>& output,
		const _STD vector<TNeuralType>& neurals)
	{
		assert(input.size() >= TNeuralType::WeightSize);
		_ResizeOutput(output, TNeuralCount);

		concurrency::array_view<const FloatingPoint, 1> inputView(TNeuralType::WeightSize, input);
		concurrency::array_view<const TNeuralType, 1> neuralsView(TNeuralCount, neurals);
		concurrency::array_view<FloatingPoint, 1> outputView(TNeuralCount, output);
		outputView.discard_data();

		concurrency::extent<2> extent(TNeuralCount, TNeuralDataSize);

		concurrency::parallel_for_each(
			extent.tile<TNeuralCount, TNeuralDataSize>(),
			[=](concurrency::tiled_index<TNeuralCount, TNeuralDataSize> idx) restrict(amp)
		{
			auto neuralIndex = idx.global[0];
			auto weightIdx = idx.global[1];
			tile_static FloatingPoint tmpData[TNeuralCount][TNeuralDataSize];

			auto& tmpNeuralData = tmpData[neuralIndex];
			auto& tmpWeightData = tmpNeuralData[weightIdx];
			// 算 权重 * 输入
			if (weightIdx == TNeuralType::WeightSize)
			{
				// 一些书里用的是正数
				tmpWeightData = -neuralsView[neuralIndex].GetThreshold();
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
				for (size_t i = 0; i < TNeuralDataSize; i++)
				{
					netWeight += tmpNeuralData[i];
				}

				// 净权重转换
				outputView[neuralIndex] = TNeuralActivationFunction::Convert(netWeight);
			}

		});
		outputView.synchronize();

	}

};

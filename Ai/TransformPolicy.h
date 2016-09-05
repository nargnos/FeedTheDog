#pragma once
#include <vector>
#include <cassert>
#include <amp.h>
#include "Define.h"
#include "Neural.h"

template<typename TNeuralActivationFunction>
struct TransformPolicy
{
	template<typename TNeuralType, int NeuralCount>
	static void Transform(
		const _STD vector<FloatingPoint>& input,
		_Out_ _STD vector<FloatingPoint>& output,
		const _STD array<TNeuralType, NeuralCount>& neurals)
	{
		assert(input.size() >= TNeuralType::WeightSize);
		if (output.size() < NeuralCount)
		{
			output.resize(NeuralCount);
		}
		
		concurrency::array_view<const FloatingPoint, 1> inputView(TNeuralType::WeightSize, input);

		concurrency::array_view<const TNeuralType, 1> neuralsView(NeuralCount, neurals);

		// �м�����(Ϊ�˷ָ���gpu�߳�)
		constexpr auto neuralDataSize = TNeuralType::WeightSize + 1;
		constexpr auto allWeightCount = NeuralCount * neuralDataSize;
		_STD vector<FloatingPoint> tmpData(allWeightCount);
		// �ݱ�ʾ��ͬ��Ԫ�����ʾԪ��Ȩ����������ֵ��
		concurrency::array_view<FloatingPoint, 2> tmpDataView(NeuralCount, neuralDataSize, tmpData);
		tmpDataView.discard_data();

		// �� Ȩ�� * Ȩ
		concurrency::parallel_for_each(tmpDataView.extent, [=](concurrency::index<2> idx) restrict(amp)
		{
			auto neuralIndex = idx[0];
			auto weightIdx = idx[1];
			if (weightIdx == TNeuralType::WeightSize)
			{
				tmpDataView[idx] = neuralsView[neuralIndex].GetThreshold();
			}
			else
			{
				auto tmp = neuralsView[neuralIndex].GetWeights()[weightIdx];
				tmp *= inputView[weightIdx];
				tmpDataView[idx] = tmp;
			}
		});
		tmpDataView.synchronize();

		concurrency::array_view<FloatingPoint, 1> outputView(NeuralCount, output);
		outputView.discard_data();

		// �� ��Ȩ�ز�ת�����
		concurrency::parallel_for_each(outputView.extent, [=](concurrency::index<1> idx) restrict(amp)
		{
			auto neuralIndex = idx[0];
			FloatingPoint netWeight = 0;
			for (size_t i = 0; i < neuralDataSize; i++)
			{
				netWeight += tmpDataView(neuralIndex, i);
			}
			// ��Ȩ��ת��
			outputView[idx] = TNeuralActivationFunction::Convert(netWeight);
		});
		outputView.synchronize();
	}
};

#pragma once
#include <memory>
#include <vector>
#include "Define.h"
#include "INeuralProxy.h"
__interface ILayer
{
	size_t GetInputSize() const;
	size_t GetNeuralCount() const;
	size_t GetNeuralWeightSize() const;
	size_t GetNeuralDataSize() const;

	size_t GetOutputSize() const;
	_STD unique_ptr<INeuralProxy> GetNeural(size_t index);
	FloatingPoint GetWeight(size_t neuralIndex, size_t weightIndex) const;
	FloatingPoint GetThreshold(size_t neuralIndex) const;
	void SetWeight(size_t neuralIndex, size_t weightIndex, FloatingPoint val);
	void SetThreshold(size_t neuralIndex, FloatingPoint val);

	void Transform(const _STD vector<FloatingPoint>& input, _Out_ _STD vector<FloatingPoint>& output) const;
	void GetNetWeightAndResult(const _STD vector<FloatingPoint>& input, _Out_ _STD vector<FloatingPoint>& netWeight, _Out_ _STD vector<FloatingPoint>& output) const;

	// ȡ���������ݶ�, �������ƽ����
	// netWeight�ᱻ�����ת��
	FloatingPoint GetOutputLayerGradients(const _STD vector<FloatingPoint>& lastOutput, const _STD vector<FloatingPoint>& netWeight, const _STD vector<FloatingPoint>& desire, _Out_ _STD vector<FloatingPoint>& gradients) const;
	// ȡ���ز�����ݶ�
	void GetHiddenLayerGradients(const _STD vector<FloatingPoint>& lastOutput, const _STD vector<FloatingPoint>& netWeight, const _STD vector<FloatingPoint>& nextLayerGradients, const ILayer& nextLayer, _Out_ _STD vector<FloatingPoint>& gradients) const;

	void UpdateWeight(const _STD vector<FloatingPoint>& input, const _STD vector<FloatingPoint>& gradients, const _STD vector<FloatingPoint>& lastWeightDelta, FloatingPoint trainingRate, FloatingPoint momentum, _Out_ _STD vector<FloatingPoint>& weightDelta);

	// �����������������gpu
	const FloatingPoint* _GetRawData() const;
};


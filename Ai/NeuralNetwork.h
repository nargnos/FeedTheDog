#pragma once
#include <memory>
#include <vector>
#include <cassert>
#include "ILayer.h"
#include "ILayerBuilder.h"
#include "INeuralNetwork.h"

class NeuralNetwork:
	public INeuralNetwork
{
public:
	NeuralNetwork() = default;
	~NeuralNetwork() = default;

	virtual size_t GetLayerCount() const override;
	virtual void AddLayer(const ILayerBuilder& builder)override;

	virtual const ILayer& GetLayer(size_t index) const override;
	virtual ILayer& GetLayer(size_t index) override;
	virtual _STD vector<FloatingPoint> Transform(const _STD vector<FloatingPoint>& input) const override;
	virtual size_t GetInputSize() const override;
	virtual size_t GetOutputSize() const override;

	virtual _STD vector<_STD vector<FloatingPoint>> GetWeights() const override;
protected:
	bool IsLayerCanAdd(_STD unique_ptr<ILayer>& layer) const;
	_STD vector<_STD unique_ptr<ILayer>> layers_;

	
};


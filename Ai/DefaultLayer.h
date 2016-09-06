#pragma once
#include "Layer.h"
#include "DefaultInputPolicy.h"
#include "TransformPolicy.h"
#include "SigmoidActivation.h"
#include "LinearActivation.h"
#include "ReluActivation.h"

template<size_t TInputSize, size_t TNeuralCount >
using SigmoidLayer = Layer<TInputSize, TNeuralCount, TransformPolicy<SigmoidActivation>, DefaultInputPolicy>;

template<size_t TInputSize, size_t TNeuralCount >
using LinearLayer = Layer<TInputSize, TNeuralCount, TransformPolicy<LinearActivation>, DefaultInputPolicy>;

template<size_t TInputSize, size_t TNeuralCount >
using ReluLayer = Layer<TInputSize, TNeuralCount, TransformPolicy<ReluActivation>, DefaultInputPolicy>;


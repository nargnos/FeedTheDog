#pragma once
#include "Layer.h"
#include "DefaultInputPolicy.h"
#include "TrainPolicy.h"
#include "TransformPolicy.h"
#include "SigmoidActivation.h"

template<size_t TInputSize, size_t TNeuralCount >
using DefaultHiddenLayer = Layer<TInputSize, TNeuralCount, TransformPolicy<SigmoidActivation>, TrainPolicy, DefaultInputPolicy>;

template<size_t TInputSize, size_t TNeuralCount >
using DefaultOutputLayer = Layer<TInputSize, TNeuralCount, TransformPolicy<SigmoidActivation>, TrainPolicy, DefaultInputPolicy>;
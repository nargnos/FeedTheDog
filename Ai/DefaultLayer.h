#pragma once
#include "Layer.h"
#include "DefaultInputPolicy.h"
#include "TrainPolicy.h"
#include "TransformPolicy.h"
#include "SigmoidActivation.h"

template<size_t NeuralCount, size_t InputSize>
using DefaultHiddenLayer = Layer<NeuralCount, InputSize, TransformPolicy<SigmoidActivation>, TrainPolicy, DefaultInputPolicy>;

template<size_t NeuralCount, size_t InputSize>
using DefaultOutputLayer = Layer<NeuralCount, InputSize, TransformPolicy<SigmoidActivation>, TrainPolicy, DefaultInputPolicy>;
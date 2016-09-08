#pragma once
#include "Layer.h"
#include "SigmoidActivation.h"
#include "LinearActivation.h"
#include "ReluActivation.h"


template<size_t TInputSize, size_t TNeuralCount>
using SigmoidLayer = Layer<TInputSize, TNeuralCount, SigmoidActivation>;

template<size_t TInputSize, size_t TNeuralCount>
using LinearLayer = Layer<TInputSize, TNeuralCount, LinearActivation>;

template<size_t TInputSize, size_t TNeuralCount>
using ReluLayer = Layer<TInputSize, TNeuralCount, ReluActivation>;


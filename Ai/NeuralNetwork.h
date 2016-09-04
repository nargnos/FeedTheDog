#pragma once
#include <initializer_list>
#include <vector>
#include <cassert>
#include <ppl.h>
#include <amp.h>
#include <amp_math.h>
#include <stack>
#include "Define.h"
#include "Linear.h"
#include "Sigmoid.h"
#include "Neuron.h"



template<typename TTransformFunction>
class Layer
{
public:

	Layer(size_t neuralCount, size_t inputSize) :
		weights_(Generate(neuralCount * inputSize, inputSize)),
		threshold_(Generate(neuralCount, inputSize)),
		neuralCount_(neuralCount),
		inputSize_(inputSize),
		lastWeightDelta_(neuralCount * inputSize),
		lastThresholdDelta_(neuralCount)
	{
		using namespace std;
		assert(neuralCount != 0 && inputSize != 0);
	}
	~Layer() = default;

	void Rebuild()
	{
		auto newWeights = Generate(neuralCount_ * inputSize_, inputSize_);
		weights_.swap(newWeights);
		auto newThreshold = Generate(neuralCount_, inputSize_);
		threshold_.swap(newThreshold);

		_STD vector<FloatingPoint> newLastWeightDelta(neuralCount * inputSize);
		_STD vector<FloatingPoint> newLastThresholdDelta(neuralCount);

		lastWeightDelta_.swap(newLastWeightDelta);
		newLastThresholdDelta.swap(newLastThresholdDelta);
	}
	size_t GetNeuralCount() const
	{
		return neuralCount_;
	}

	size_t GetInputSize() const
	{
		return inputSize_;
	}
	const _STD vector<FloatingPoint>& GetWeights() const
	{
		return weights_;
	}
	const _STD vector<FloatingPoint>& GetThreshold() const
	{
		return threshold_;
	}
	_STD vector<FloatingPoint>& GetWeights()
	{
		return weights_;
	}
	_STD vector<FloatingPoint>& GetThreshold()
	{
		return threshold_;
	}
	void Transform(
		const _STD vector<FloatingPoint>& input,
		_STD vector<FloatingPoint>& result) const
	{
		TTransformFunction::ParallelTransform(
			input,
			weights_,
			threshold_,
			GetNeuralCount(),
			GetInputSize(),
			result);
	}
	void TrainLayer(
		const _STD vector<FloatingPoint>& input,
		const _STD vector<FloatingPoint>& gradient,
		FloatingPoint trainingRate)
	{
		assert(GetInputSize() == input.size());
		assert(GetNeuralCount() == gradient.size());

		TTransformFunction::ParallelTrainLayer(input, gradient, weights_, threshold_, lastWeightDelta_, lastThresholdDelta_, trainingRate);
	}
private:
	static FloatingPoint GetMaxWeight(int inputSize)
	{
		// 书里说用2.4
		return 2.4f / inputSize;
	}
	static FloatingPoint GetMinWeight(int inputSize)
	{
		return -GetMaxWeight(inputSize);
	}
	static _STD vector<FloatingPoint> Generate(size_t size, size_t inputSize)
	{
		_STD vector<FloatingPoint> vec(size);
		_STD generate(
			begin(vec),
			end(vec),
			[min = GetMinWeight(inputSize), max = GetMaxWeight(inputSize)]()
		{
			return _GenerateWeight(min, max);
		});
		return vec;
	}
	// 2维存1维
	// 行为权（等于输入大小），列为元（等于阈值个数）
	mutable _STD vector<FloatingPoint> weights_;
	mutable _STD vector<FloatingPoint> threshold_;
	mutable _STD vector<FloatingPoint> lastWeightDelta_;
	mutable _STD vector<FloatingPoint> lastThresholdDelta_;
	size_t neuralCount_;
	size_t inputSize_;
};

class NoName
{
public:
	static void ParallelTransform(
		const _STD vector<FloatingPoint>& input,
		const _STD vector<FloatingPoint>& weights,
		const _STD vector<FloatingPoint>& threshold,
		size_t neuralCount,
		size_t inputSize,
		_STD vector<FloatingPoint>& result)
	{
		using namespace std;

		auto& colLen = neuralCount;
		auto& rowLen = inputSize;

		assert(input.size() >= rowLen);
		assert(result.size() >= colLen);
		vector<FloatingPoint> tmpWeight(const_cast<_STD vector<FloatingPoint>&>(weights));

		concurrency::array_view<FloatingPoint, 2> weightView(colLen, rowLen, tmpWeight);
		concurrency::array_view<FloatingPoint, 1> inputView(rowLen, const_cast<_STD vector<FloatingPoint>&>(input));
		concurrency::array_view<FloatingPoint, 1> thresholdView(colLen, const_cast<_STD vector<FloatingPoint>&>(threshold));


		concurrency::parallel_for_each(
			weightView.extent,
			[=](concurrency::index<2> idx) restrict(amp)
		{
			weightView[idx] *= inputView[idx[1]];
		});
		weightView.synchronize();
		concurrency::array_view<FloatingPoint, 1> resultView(colLen, result);
		concurrency::parallel_for_each(
			resultView.extent,
			[=](concurrency::index<1> idx) restrict(amp)
		{
			FloatingPoint tmpResult = 0;
			for (size_t i = 0; i < rowLen; i++)
			{
				tmpResult += weightView(idx[0], i);
			}

			// 执行转换, 并存储返回值
			resultView[idx] = Convert(tmpResult - thresholdView[idx]);

		});
		resultView.synchronize();
	}


	static FloatingPoint ParallelTrain(
		const _STD vector<FloatingPoint> & input,
		_STD vector<Layer<NoName>>& hiddenLayer,
		Layer<NoName>& outputLayer,
		const _STD vector<FloatingPoint>& desire,
		FloatingPoint trainingRate)
	{
		using namespace std;
		assert(outputLayer.GetNeuralCount() == desire.size());

		auto hiddenLayerResult = GetHiddenLayerResult(hiddenLayer, input);
		auto outputLayerResult = GetOutputLayerResult(outputLayer, hiddenLayerResult.empty() ? input : hiddenLayerResult.back());

		FloatingPoint deviation = 0;
		// 反向传播

		// 误差梯度个数 = 神经元个数
		auto outputlayerGradient = GetOutputLayerGradient(outputLayerResult, desire, deviation);
		// 注意下标跟layer相反
		auto hiddenLayerGradient = GetHiddenLayerGradient(hiddenLayer, hiddenLayerResult, outputLayer, outputlayerGradient);


		auto hiddenLayerSize = hiddenLayer.size();
		concurrency::parallel_invoke([&]()
		{
			outputLayer.TrainLayer(hiddenLayerResult.empty() ? input : hiddenLayerResult.back(), outputlayerGradient, trainingRate);
		}, [&]()
		{
			concurrency::parallel_for(0u, hiddenLayerSize, [&](size_t i)
			{
				auto& inputRef = i == 0 ? input : hiddenLayerResult[i - 1];
				hiddenLayer[i].TrainLayer(inputRef, hiddenLayerGradient[hiddenLayerSize - 1 - i], trainingRate);
			});
		});

		return deviation;
	}
	static void ParallelTrainLayer(
		const _STD vector<FloatingPoint>& input,
		const _STD vector<FloatingPoint>& gradient,
		_STD vector<FloatingPoint>& weights,
		_STD vector<FloatingPoint>& threshold,
		_STD vector<FloatingPoint>& lastWeightDelta,
		_STD vector<FloatingPoint>& lastThresholdDelta,
		FloatingPoint trainingRate)
	{
		using namespace std;
		using namespace concurrency;

		CHECK_ALPHA(trainingRate);
		auto inputSize = input.size();
		auto gradientSize = gradient.size();
		auto thresholdSize = threshold.size();

		assert(weights.size() / thresholdSize == inputSize);
		assert(threshold.size() == gradientSize);

		array_view<FloatingPoint, 1> inputView(inputSize, const_cast<_STD vector<FloatingPoint>&>(input));
		array_view<FloatingPoint, 1> gradientView(gradientSize, const_cast<_STD vector<FloatingPoint>&>(gradient));
		array_view<FloatingPoint, 2> weightsView(thresholdSize, inputSize, weights);
		array_view<FloatingPoint, 1> thresholdView(thresholdSize, threshold);

		array_view<FloatingPoint, 2> lastWeightDeltaView(thresholdSize, inputSize, lastWeightDelta);
		array_view<FloatingPoint, 1> lastThresholdDeltaView(thresholdSize, lastThresholdDelta);

		constexpr FloatingPoint beta(0.95);
		// 计算delta并修改权
		parallel_invoke([&]()
		{
			parallel_for_each(
				weightsView.extent,
				[=](concurrency::index<2> idx) restrict(amp)
			{
				auto delta = trainingRate * inputView(idx[1]) * gradientView(idx[0]);
				delta = beta * lastWeightDeltaView[idx] + delta;

				weightsView[idx] += delta;
				lastWeightDeltaView[idx] = delta;
			});
			lastWeightDeltaView.synchronize();
			weightsView.synchronize();
		}, [&]()
		{
			parallel_for_each(
				thresholdView.extent,
				[=, rate = -trainingRate](concurrency::index<1> idx) restrict(amp)
			{
				auto delta = rate * gradientView(idx[0]);
				delta = beta * lastThresholdDeltaView[idx] + delta;

				thresholdView[idx] += delta;
				lastThresholdDeltaView[idx] = delta;
			});
			lastThresholdDeltaView.synchronize();
			thresholdView.synchronize();
		});


	}

protected:
	static _STD vector<_STD vector<FloatingPoint>> GetHiddenLayerResult(
		const _STD vector<Layer<NoName>>& hiddenLayer,
		const _STD vector<FloatingPoint> & input)
	{
		using namespace std;
		vector<vector<FloatingPoint>> result;
		if (!hiddenLayer.empty())
		{
			result.reserve(hiddenLayer.size());

			auto inputBuffer = &input;

			// 这里貌似只能串行
			for (auto& layer : hiddenLayer)
			{
				// 有几个神经元就有几个返回
				result.emplace_back(layer.GetNeuralCount());

				layer.Transform(*inputBuffer, result.back());
				inputBuffer = &result.back();
			}
		}

		return result;
	}

	static _STD vector<FloatingPoint> GetOutputLayerResult(
		const Layer<NoName>& outputLayer,
		const _STD vector<FloatingPoint>& input)
	{
		_STD vector<FloatingPoint> result(outputLayer.GetNeuralCount());
		outputLayer.Transform(input, result);
		return result;
	}

	static _STD vector<FloatingPoint> GetOutputLayerGradient(
		const _STD vector<FloatingPoint>& lastOutput,
		const _STD vector<FloatingPoint>& desire,
		_Out_ FloatingPoint& deviation)
	{
		using namespace std;
		// 复制期望到梯度暂存
		auto desireSize = desire.size();
		_STD vector<FloatingPoint> gradient(desire);
		_STD vector<FloatingPoint> deviationResult(desireSize);

		concurrency::array_view<FloatingPoint, 1> gradientView(desireSize, gradient);
		concurrency::array_view<FloatingPoint, 1> lastOutputView(desireSize, const_cast<_STD vector<FloatingPoint>&>(lastOutput));
		concurrency::array_view<FloatingPoint, 1> deviationView(desireSize, deviationResult);


		concurrency::parallel_for_each(
			gradientView.extent,
			[=](concurrency::index<1> idx) restrict(amp)
		{
			auto output = lastOutputView[idx];
			auto tmpDesire = gradientView[idx];
			auto tmpDeviation = tmpDesire - output;
			gradientView[idx] = GetGradient(output, tmpDeviation);
			deviationView[idx] = MATH_NAMESPACE pow(tmpDeviation, 2);
		});

		gradientView.synchronize();
		deviationView.synchronize();

		deviation = accumulate(begin(deviationResult), end(deviationResult), _STD vector<FloatingPoint>::value_type(0));
		return gradient;
	}
	// 注意这里返回的HiddenLayerGradient下标是反的
	static _STD vector<_STD vector<FloatingPoint>> GetHiddenLayerGradient(
		const _STD vector<Layer<NoName>>& hiddenLayer,
		const _STD vector<_STD vector<FloatingPoint>>& hiddenLayerResult,
		const Layer<NoName>& outputLayer,
		const _STD vector<FloatingPoint>& outputlayerGradient)
	{
		using namespace std;
		using namespace concurrency;

		vector<vector<FloatingPoint>> gradientResult;
		if (!hiddenLayer.empty())
		{
			auto hiddenLayerCount = hiddenLayer.size();
			gradientResult.reserve(hiddenLayerCount);

			assert(outputLayer.GetNeuralCount() == outputlayerGradient.size());

			auto behindLayerGradient = const_cast<vector<FloatingPoint>*>(&outputlayerGradient);
			auto behindLayerWeights = const_cast<vector<FloatingPoint>*>(&outputLayer.GetWeights());
			auto behindLayer = const_cast<Layer<NoName>*>(&outputLayer);

			for (size_t i = hiddenLayerCount; i > 0; i--)
			{
				size_t index = i - 1;
				auto& layer = hiddenLayer[index];
				auto& layerResult = const_cast<vector<FloatingPoint>&>(hiddenLayerResult[index]);
				assert(behindLayer->GetInputSize() == layer.GetNeuralCount());

				auto layerNeuralCount = layer.GetNeuralCount();
				// 注意这里导致梯度下标跟层下标相反
				gradientResult.emplace_back(layerNeuralCount);
				auto& gradient = gradientResult.back();

				auto behindLayerNeuralCount = behindLayer->GetNeuralCount();
				array_view<FloatingPoint, 1> behindLayerGradientView(behindLayerNeuralCount, *behindLayerGradient);
				array_view<FloatingPoint, 2> behindLayerWeightView(layerNeuralCount, behindLayerNeuralCount, *behindLayerWeights);
				array_view<FloatingPoint, 1> layerResultView(layerNeuralCount, layerResult);
				array_view<FloatingPoint, 1> gradientView(layerNeuralCount, gradient);


				parallel_for_each(
					gradientView.extent,
					[=](concurrency::index<1> idx) restrict(amp)
				{
					float tmp = 0;
					// 计算下层的 梯度*权 之和
					for (size_t i = 0; i < behindLayerNeuralCount; i++)
					{
						tmp += behindLayerGradientView(i) * behindLayerWeightView(idx[0], i);
					}
					gradientView[idx] = GetGradient(layerResultView[idx], tmp);
				});
				gradientView.synchronize();

				behindLayerGradient = &gradient;
				behindLayerWeights = const_cast<vector<FloatingPoint>*>(&layer.GetWeights());
				behindLayer = const_cast<Layer<NoName>*>(&layer);
			}
		}


		return gradientResult;
	}
	static FloatingPoint GetGradient(FloatingPoint output, FloatingPoint deviation) restrict(amp)
	{
		return N_Npow2(output) * deviation;
	}
	// N * (1 - N)
	static FloatingPoint N_Npow2(FloatingPoint n) restrict(amp)
	{
		return n - MATH_NAMESPACE pow(n, 2);
	}

	static FloatingPoint Convert(FloatingPoint x) restrict(amp)
	{
#define USE_SIGMOID 1
#ifdef USE_SIGMOID
		// sigmoid
		return 1 / (1 + MATH_NAMESPACE exp(-x));
#else
		// 使用这个貌似要把梯度计算改掉，需要改成这个函数的导数*误差
		constexpr FloatingPoint a(1.716);
		constexpr FloatingPoint b(0.667);
		// tanh
		return -a + 2 * a / (1 + MATH_NAMESPACE exp(-b * x));
#endif // USE_SIGMOID

	}
};




class NeuralNetworkDesire
{
public:
	NeuralNetworkDesire(
		const _STD initializer_list<FloatingPoint>& input,
		const _STD initializer_list<FloatingPoint>& desire,
		FloatingPoint trainingRate = 0.1) :
		input_(input),
		desire_(desire),
		trainingRate_(trainingRate),
		lastDeviation_(-1)
	{
	}

	~NeuralNetworkDesire() = default;


	FloatingPoint ParallelTrain(
		_STD vector<Layer<NoName>>& hiddenLayer,
		Layer<NoName>& outputLayer)
	{
		auto result = NoName::ParallelTrain(input_, hiddenLayer, outputLayer, desire_, trainingRate_);
		if (lastDeviation_ > 0)
		{
			// 调节学习速率
			if (result > lastDeviation_ * FloatingPoint(1.04))
			{
				trainingRate_ *= FloatingPoint(0.7);
			}
			else if (result < lastDeviation_)
			{
				trainingRate_ *= FloatingPoint(1.05);
			}
		}

		lastDeviation_ = result;
		return lastDeviation_;
	}
	const _STD vector<FloatingPoint>& GetInput() const
	{
		return input_;
	}
	const _STD vector<FloatingPoint>& GetDesire() const
	{
		return desire_;
	}
protected:
	_STD vector<FloatingPoint> input_;
	_STD vector<FloatingPoint> desire_;
	FloatingPoint trainingRate_;
	FloatingPoint lastDeviation_;
};

class NeuralNetworkBase
{
public:
	NeuralNetworkBase(
		size_t inputLayerSize,
		const _STD initializer_list<size_t>& hiddenLayerSize,
		size_t outputLayerSize) :
		outputLayer_(outputLayerSize, hiddenLayerSize.size() > 0 ? *_STD rbegin(hiddenLayerSize) : inputLayerSize)
	{
		using namespace std;

		// 存储层中最大有几个神经元
		maxNeuralCount_ = hiddenLayerSize.size() > 0 ? *max_element(begin(hiddenLayerSize), end(hiddenLayerSize)) : 0;
		auto tmpCounts = { inputLayerSize, outputLayerSize, maxNeuralCount_ };
		maxNeuralCount_ = *max_element(begin(tmpCounts), end(tmpCounts));

		hiddenLayer_.reserve(hiddenLayerSize.size());
		auto tmpInputSize = inputLayerSize;
		for (auto val : hiddenLayerSize)
		{
			assert(val > 0);
			hiddenLayer_.emplace_back(val, tmpInputSize);
			tmpInputSize = val;
		}
	}
	virtual ~NeuralNetworkBase() = default;

	// 注意类型长度
	_STD vector<FloatingPoint> Transform(const _STD vector<FloatingPoint> & input) const
	{
		using namespace std;
		// 双buffer交替做输入输出
		vector<FloatingPoint> buffer1(input);
		buffer1.resize(maxNeuralCount_);
		vector<FloatingPoint> buffer2(maxNeuralCount_);

		auto inputBuffer = &buffer1;
		auto outputBuffer = &buffer2;

		for (auto& layer : hiddenLayer_)
		{
			layer.Transform(*inputBuffer, *outputBuffer);
			swap(inputBuffer, outputBuffer);
		}
		outputLayer_.Transform(*inputBuffer, *outputBuffer);
		return _STD vector<FloatingPoint>(outputBuffer->begin(), outputBuffer->begin() + outputLayer_.GetNeuralCount());
	}

	// 返回值为误差平方和
	template<typename TDesire>
	FloatingPoint Train(TDesire& desire)
	{
		return desire.ParallelTrain(
			hiddenLayer_,
			outputLayer_);
	}
	_STD vector<Layer<NoName>>& GetHiddenLayer()
	{
		return hiddenLayer_;
	}
	Layer<NoName>& GetOutputLayer()
	{
		return outputLayer_;
	}
	const _STD vector<Layer<NoName>>& GetHiddenLayer() const
	{
		return hiddenLayer_;
	}
	const Layer<NoName>& GetOutputLayer() const
	{
		return outputLayer_;
	}
protected:
	_STD vector<Layer<NoName>> hiddenLayer_;
	Layer<NoName> outputLayer_;
	FloatingPoint desireSize_;
	size_t maxNeuralCount_;
};


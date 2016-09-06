#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include <amp.h>
#include <array>
#include <vector>
#include <Ai\NeuralNetwork.h>
#include <Ai\DefaultLayer.h>
#include <Ai\RandomLayerBuilder.h>
#include <Ai\ValueLayerBuilder.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AiTest
{
	using namespace std;
	struct X
	{
		int v3;
		float v4;
		double v5;
		std::array<int, 3> f;
		//std::vector<int> l;
		void XX() restrict(amp)
		{
			v5 = 99;
		}
	};
	TEST_CLASS(AiTest)
	{
	public:
		TEST_METHOD(AmpTest)
		{
			vector<X> vec(10);
			concurrency::array_view<X, 1> view(vec.size(), vec);

			concurrency::parallel_for_each(view.extent, [=](concurrency::index<1> idx) restrict(amp)
			{
				auto& val = view[idx];
				val.v3 = 1;
				val.v4 = 1;
				val.v5 = 1;
				val.XX();

			});
			view.synchronize();
		}

		TEST_METHOD(NeuralNetworkTest)
		{
			using namespace std;
			NeuralNetwork nn;
			//RandomLayerBuilder<DefaultHiddenLayer<2, 20>> hiddenLayerBuilder;
			//RandomLayerBuilder<DefaultOutputLayer<20, 1>> outputLayerBuilder;

			using HiddenLayerBuilder = ValueLayerBuilder<DefaultHiddenLayer<2, 2>>;
			HiddenLayerBuilder hiddenLayerBuilder
			({
				{0.5f, 0.4f, 0.8f},
				{0.9f, 1.f, -0.1f}
			});

			using OutputLayerBuilder = ValueLayerBuilder<DefaultOutputLayer<2,1>>;
			OutputLayerBuilder outputLayerBuilder
			({
				{-1.2f, 1.1f, 0.3f}
			});


			nn.AddLayer(hiddenLayerBuilder);
			nn.AddLayer(outputLayerBuilder);

			vector<FloatingPoint> input{ 1,1 };
			auto x = nn.Transform(input);
			//nn.GetLayer(0).
		}


		// 感知器测试
		//TEST_METHOD(PerceptronTest)
		//{
		//	using namespace std;

		//	// 训练位运算
		//	constexpr int inputSize = 2;
		//	constexpr int dataSize = 4;

		//	typedef std::array<int, inputSize> Input;

		//	std::array<Input, dataSize> inputs
		//	{
		//		Input{0,0},
		//		Input{0,1},
		//		Input{1,0},
		//		Input{1,1},
		//	};

		//	// or
		//	std::array<PerceptronDesire, dataSize> orResults{ 0,1,1,1 };

		//	auto func = [](auto val)
		//	{
		//		return val > 0.9;
		//	};

		//	Perceptron orTest(2);
		//	for (size_t i = 0; i < 10; i++)
		//	{
		//		for (size_t j = 0; j < dataSize; j++)
		//		{
		//			orTest.Train(inputs[j], orResults[j], func);
		//		}
		//	}

		//	// and
		//	std::array<PerceptronDesire, dataSize> andResults{ 0,0,0,1 };
		//	Perceptron andTest(2);
		//	float rate = 0.9f;
		//	for (size_t i = 0; i < 10; i++)
		//	{
		//		FloatingPoint deviation = 0;
		//		for (size_t j = 0; j < dataSize; j++)
		//		{
		//			deviation += abs(andTest.Train(inputs[j], andResults[j], func, rate));
		//		}
		//		if (deviation < 0.0001)
		//		{
		//			break;
		//		}
		//		if (deviation < 5)
		//		{
		//			rate = rate * 0.5f;
		//			if (rate < 0.01)
		//			{
		//				break;
		//			}
		//		}
		//	}

		//	// 验证
		//	for (auto& val : inputs)
		//	{
		//		// or
		//		Assert::IsTrue((bool)func(orTest.Transform(val)) == (val[0] || val[1]));
		//		// and
		//		Assert::IsTrue((bool)func(andTest.Transform(val)) == (val[0] && val[1]));
		//	}


		//	// 输出
		//	std::ostringstream output;

		//	auto testResult = [&func]
		//	(std::ostringstream& output, std::array<Input, dataSize>& inputs, Perceptron& p)
		//	{
		//		for (auto& input : inputs)
		//		{
		//			for (auto val : input)
		//			{
		//				output << (bool)val << " ";
		//			}
		//			output << "-> " << (bool)func(p.Transform(input)) << std::endl;
		//		}

		//		output << "权重们: ";
		//		for (auto val : p.GetWeights())
		//		{
		//			output << val << ", ";
		//		}

		//		output << std::endl;
		//		output << "阈值: " << p.GetThreshold() << std::endl;
		//	};

		//	output << "或操作" << endl;
		//	testResult(output, inputs, orTest);
		//	output << endl << "与操作" << endl;
		//	testResult(output, inputs, andTest);


		//	Logger::WriteMessage(output.str().c_str());

		//}
		//TEST_METHOD(NeuralNetworkTest)
		//{
		//	// 训练位运算
		//	constexpr int inputSize = 2;
		//	constexpr int dataSize = 4;

		//	NeuralNetworkBase nn(2, { 2 }, 1);
		//	// xor
		//
		//	std::vector<NeuralNetworkDesire> xorTrain
		//	{
		//		NeuralNetworkDesire({0,0},{0}),
		//		NeuralNetworkDesire({0,1},{1}),
		//		NeuralNetworkDesire({1,0},{1}),
		//		NeuralNetworkDesire({1,1},{0}),
		//	};
		//	nn.GetOutputLayer().GetWeights() = { -10.3788f,9.7691f };
		//	nn.GetOutputLayer().GetThreshold() = { 4.5589f };

		//	nn.GetHiddenLayer()[0].GetWeights() = { 4.7621f,4.7618f,6.3917f,6.3917f };
		//	nn.GetHiddenLayer()[0].GetThreshold() = { 7.3061f, 2.8441f };


		//	auto t = clock();
		//	FloatingPoint d = 0;

		//	for (size_t i = 0; i < 500; i++)
		//	{
		//		d = 0;
		//		for (auto& val : xorTrain)
		//		{
		//			d += nn.Train(val);
		//		}
		//		if (d < 0.001)
		//		{
		//			break;
		//		}
		//	}

		//	t = clock() - t;
		//	std::ostringstream output;
		//	output << "最终误差: " << d << "\t" << "训练用时: " << t << std::endl;
		//	Logger::WriteMessage(output.str().c_str());
		//	for (auto& val : xorTrain)
		//	{
		//		auto res = nn.Transform(val.GetInput());
		//	}


		//}
	};
}
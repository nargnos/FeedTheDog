#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include <array>
#include <Ai\Perceptron.h>
#include <Ai\NeuralNetwork.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AiTest
{
	TEST_CLASS(AiTest)
	{
	public:
		// ��֪������
		TEST_METHOD(PerceptronTest)
		{
			using namespace std;

			// ѵ��λ����
			constexpr int inputSize = 2;
			constexpr int dataSize = 4;

			typedef std::array<int, inputSize> Input;

			std::array<Input, dataSize> inputs
			{
				Input{0,0},
				Input{0,1},
				Input{1,0},
				Input{1,1},
			};

			// or
			std::array<PerceptronDesire, dataSize> orResults{ 0,1,1,1 };

			auto func = [](auto val)
			{
				return val > 0.9;
			};

			Perceptron orTest(2);
			for (size_t i = 0; i < 10; i++)
			{
				for (size_t j = 0; j < dataSize; j++)
				{
					orTest.Train(inputs[j], orResults[j], func);
				}
			}

			// and
			std::array<PerceptronDesire, dataSize> andResults{ 0,0,0,1 };
			Perceptron andTest(2);
			float rate = 0.9f;
			for (size_t i = 0; i < 10; i++)
			{
				FloatingPoint deviation = 0;
				for (size_t j = 0; j < dataSize; j++)
				{
					deviation += abs(andTest.Train(inputs[j], andResults[j], func, rate));
				}
				if (deviation < 0.0001)
				{
					break;
				}
				if (deviation < 5)
				{
					rate = rate * 0.5f;
					if (rate < 0.01)
					{
						break;
					}
				}
			}

			// ��֤
			for (auto& val : inputs)
			{
				// or
				Assert::IsTrue((bool)func(orTest.Transform(val)) == (val[0] || val[1]));
				// and
				Assert::IsTrue((bool)func(andTest.Transform(val)) == (val[0] && val[1]));
			}


			// ���
			std::ostringstream output;

			auto testResult = [&func]
			(std::ostringstream& output, std::array<Input, dataSize>& inputs, Perceptron& p)
			{
				for (auto& input : inputs)
				{
					for (auto val : input)
					{
						output << (bool)val << " ";
					}
					output << "-> " << (bool)func(p.Transform(input)) << std::endl;
				}

				output << "Ȩ����: ";
				for (auto val : p.GetWeights())
				{
					output << val << ", ";
				}

				output << std::endl;
				output << "��ֵ: " << p.GetThreshold() << std::endl;
			};

			output << "�����" << endl;
			testResult(output, inputs, orTest);
			output << endl << "�����" << endl;
			testResult(output, inputs, andTest);


			Logger::WriteMessage(output.str().c_str());

		}
		TEST_METHOD(NeuralNetworkTest)
		{
			// ѵ��λ����
			constexpr int inputSize = 2;
			constexpr int dataSize = 4;

			NeuralNetworkBase nn(2, { 2 }, 1);
			// xor
		
			std::vector<NeuralNetworkDesire> xorTrain
			{
				NeuralNetworkDesire({0,0},{0}),
				NeuralNetworkDesire({0,1},{1}),
				NeuralNetworkDesire({1,0},{1}),
				NeuralNetworkDesire({1,1},{0}),
			};
			nn.GetOutputLayer().GetWeights() = { -10.3788f,9.7691f };
			nn.GetOutputLayer().GetThreshold() = { 4.5589f };

			nn.GetHiddenLayer()[0].GetWeights() = { 4.7621f,4.7618f,6.3917f,6.3917f };
			nn.GetHiddenLayer()[0].GetThreshold() = { 7.3061f, 2.8441f };


			auto t = clock();
			FloatingPoint d = 0;

			for (size_t i = 0; i < 500; i++)
			{
				d = 0;
				for (auto& val : xorTrain)
				{
					d += nn.Train(val);
				}
				if (d < 0.001)
				{
					break;
				}
			}

			t = clock() - t;
			std::ostringstream output;
			output << "�������: " << d << "\t" << "ѵ����ʱ: " << t << std::endl;
			Logger::WriteMessage(output.str().c_str());
			for (auto& val : xorTrain)
			{
				auto res = nn.Transform(val.GetInput());
			}


		}
	};
}
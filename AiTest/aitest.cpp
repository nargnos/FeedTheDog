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
#include <Ai\BinaryStepActivation.h>
#include <Ai\Sample.h>
#include <Ai\Trainer.h>
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
		// ¸ÐÖªÆ÷²âÊÔ
		TEST_METHOD(PerceptronTest)
		{
			NeuralNetwork nn;
			using StepLayer = Layer<2, 1, BinaryStepActivation>;
			RandomLayerBuilder<StepLayer> builder;
			nn.AddLayer(builder);


		}
		TEST_METHOD(NeuralNetworkTest)
		{
			using namespace std;
			RandomLayerBuilder<SigmoidLayer<2, 2>> hiddenLayerBuilder;
			RandomLayerBuilder<ReluLayer<2, 1>> outputLayerBuilder;

			// ÑµÁ·
			//using HiddenLayerBuilder = ValueLayerBuilder<ReluLayer<2, 2>>;
			//HiddenLayerBuilder hiddenLayerBuilder
			//({
			//	{0.5f, 0.4f, 0.8f},
			//	{0.9f, 1.f, -0.1f}
			//});
			////ReluLayer
			//using OutputLayerBuilder = ValueLayerBuilder<ReluLayer<2, 1>>;
			//OutputLayerBuilder outputLayerBuilder
			//({
			//	{-1.2f, 1.1f, 0.3f}
			//});


			_STD shared_ptr<INeuralNetwork> nn = _STD make_shared<NeuralNetwork>();
			nn->AddLayer(hiddenLayerBuilder);
			nn->AddLayer(outputLayerBuilder);

			_STD shared_ptr<ISample> sample00(new Sample({ 0,0 }, { 0 }));
			_STD shared_ptr<ISample> sample01(new Sample({ 0,1 }, { 1 }));
			_STD shared_ptr<ISample> sample10(new Sample({ 1,0 }, { 1 }));
			_STD shared_ptr<ISample> sample11(new Sample({ 1,1 }, { 0 }));
			Trainer t(nn, 0.1f, 0.2f);
			t.AddSample(sample00);
			t.AddSample(sample01);
			t.AddSample(sample10);
			t.AddSample(sample11);

			FloatingPoint d = 0;
			for (size_t i = 0; i < 200; i++)
			{
				d = t.Train();
				if (d < 0.01f)
				{
					break;
				}
			}

			// ²âÊÔ½á¹û
			auto res00 = nn->Transform({ 0,0 });;
			auto res01 = nn->Transform({ 0,1 });;
			auto res10 = nn->Transform({ 1,0 });;
			auto res11 = nn->Transform({ 1,1 });;

		}

	};
}
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
		// 感知器测试
		TEST_METHOD(PerceptronTest)
		{
			_STD shared_ptr<INeuralNetwork> nn = _STD make_shared<NeuralNetwork>();

			RandomLayerBuilder<SigmoidLayer<2, 1>> builder;
			nn->AddLayer(builder);
			_STD shared_ptr<ISample> sample00(new Sample({ 0,0 }, { 0 }));
			_STD shared_ptr<ISample> sample01(new Sample({ 0,1 }, { 0 }));
			_STD shared_ptr<ISample> sample10(new Sample({ 1,0 }, { 0 }));
			_STD shared_ptr<ISample> sample11(new Sample({ 1,1 }, { 1 }));
			Trainer t(nn, 0.1f, 0.f);
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
			if (d > 0.01f)
			{
				Logger::WriteMessage("未能收敛");
				return;
			}
			// 测试结果
			auto res00 = nn->Transform({ 0,0 });
			auto res01 = nn->Transform({ 0,1 });
			auto res10 = nn->Transform({ 1,0 });
			auto res11 = nn->Transform({ 1,1 });

			auto weights = nn->GetWeights();
		}
		TEST_METHOD(NeuralNetworkTest)
		{
			using namespace std;
			//RandomLayerBuilder<ReluLayer<2, 2>> hiddenLayerBuilder;
			//RandomLayerBuilder<ReluLayer<2, 1>> outputLayerBuilder;

			// 已经得到结果的数据
			using HiddenLayerBuilder = ValueLayerBuilder<ReluLayer<2, 2>>;
			HiddenLayerBuilder hiddenLayerBuilder
			({
				{-3.48718596f, -3.26283479f, 6.30773020f},
				{-4.23656368f, -3.78345490f, -1.85390472f}
			});

			using OutputLayerBuilder = ValueLayerBuilder<ReluLayer<2, 1>>;
			OutputLayerBuilder outputLayerBuilder
			({
				{2.98396254f, -1.47652149f, -7.42853165f}
			});


			_STD shared_ptr<INeuralNetwork> nn = _STD make_shared<NeuralNetwork>();
			nn->AddLayer(hiddenLayerBuilder);
			nn->AddLayer(outputLayerBuilder);

			_STD shared_ptr<ISample> sample00(new Sample({ 0,0 }, { 0 }));
			_STD shared_ptr<ISample> sample01(new Sample({ 0,1 }, { 1 }));
			_STD shared_ptr<ISample> sample10(new Sample({ 1,0 }, { 1 }));
			_STD shared_ptr<ISample> sample11(new Sample({ 1,1 }, { 0 }));
			Trainer t(nn, 0.1f, 0.95f);
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
			if (d > 0.01f)
			{
				Logger::WriteMessage("未能收敛");
				return;
			}
			// 测试结果
			auto res00 = nn->Transform({ 0,0 });
			auto res01 = nn->Transform({ 0,1 });
			auto res10 = nn->Transform({ 1,0 });
			auto res11 = nn->Transform({ 1,1 });

			auto weights = nn->GetWeights();

		}

	};
}
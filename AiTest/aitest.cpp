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
				if (d < 0.01f || _STD isnan(d))
				{
					break;
				}
			}
			if (d > 0.01f || _STD isnan(d))
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
			RandomLayerBuilder<ReluLayer<2, 2>> hiddenLayerBuilder;
			RandomLayerBuilder<ReluLayer<2, 1>> outputLayerBuilder;

			// 已经得到结果的数据
			/*using HiddenLayerBuilder = ValueLayerBuilder<ReluLayer<2, 2>>;
			HiddenLayerBuilder hiddenLayerBuilder
			({
				{-3.48718596f, -3.26283479f, 6.30773020f},
				{-4.23656368f, -3.78345490f, -1.85390472f}
			});

			using OutputLayerBuilder = ValueLayerBuilder<ReluLayer<2, 1>>;
			OutputLayerBuilder outputLayerBuilder
			({
				{2.98396254f, -1.47652149f, -7.42853165f}
			});*/


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
				if (d < 0.01f || _STD isnan(d))
				{
					break;
				}
			}
			if (d > 0.01f || _STD isnan(d))
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
		TEST_METHOD(NeuralNetworkTest2)
		{
			using namespace std;
			//RandomLayerBuilder<ReluLayer<15, 15>> hiddenLayerBuilder;

			//RandomLayerBuilder<SigmoidLayer<15, 4>> outputLayerBuilder;
			/*
			111
			101
			111
			101
			111
			*/

			// 已经得到结果的数据（经常nan，好歹也有收敛的结果，可能编错了什么，之后看完书再修改）
			using HiddenLayerBuilder = ValueLayerBuilder<ReluLayer<15, 15>>;
			HiddenLayerBuilder hiddenLayerBuilder
			({
				0.885234f, 1.06492f, 0.156779f, 0.819395f, -0.11585f, -0.328212f, -0.923853f, -0.700961f, -0.0193718f,
				-0.379447f, 0.0977162f, 0.0371061f, -0.822425f, -0.646164f, -0.042046f, -0.188014f, 0.100318f, -0.0598612f,
				0.0522712f, -0.0949085f, 0.0810066f, -0.113206f, -0.0803596f, -0.0560745f, 0.028313f, -0.101376f, 0.13408f,
				0.0272412f, 0.123272f, -0.0698307f, 0.045899f, 0.11161f, -0.825365f, 0.666565f, 0.251307f, 0.466826f, 0.0424485f,
				0.996938f, -0.324506f, 0.72273f, 0.129713f, -1.49704f, -0.153379f, 0.675458f, 1.01155f, 1.03267f, -0.0184683f,
				-0.0253624f, 0.309861f, 0.340889f, -0.0521048f, -1.56355f, -0.0177732f, 0.78975f, 0.160108f, 0.366809f, 0.0488047f,
				-0.784452f, -0.0299519f, -0.35327f, 0.558464f, 0.589518f, -0.215508f, 0.220354f, 0.628222f, 1.4273f, 0.22772f,
				-2.12184f, 0.0647091f, 0.990776f, -0.96465f, -0.106275f, 0.422659f, -1.33361f, -0.121581f, -0.166765f, 0.111055f,
				0.0126527f, 0.408899f, -0.41979f, 0.511727f, -0.4746f, 0.187865f, 0.203644f, -0.00792712f, 1.14845f, 0.57227f,
				0.204885f, 0.238699f, 0.827382f, -0.124274f, 0.00813633f, -0.511892f, -0.472291f, 0.327791f, -0.137504f, -0.0433989f,
				-0.0529737f, -0.134174f, 0.05038f, -0.0130142f, -0.105096f, 0.10996f, -0.104149f, -0.00397166f, -0.0697458f,
				-0.0924285f, 0.0732966f, 0.145166f, -0.0603719f, 0.0430456f, 0.0286215f, 0.22962f, 0.0323715f, 0.00655443f, -1.81708f,
				0.052129f, 0.251551f, 0.250601f, 1.61666f, 0.141726f, 2.30998f, 0.124786f, -0.624508f, 0.182983f, -0.00865494f,
				0.0364829f, 0.0521011f, -0.116143f, 0.110948f, -0.0219655f, -0.0179448f, -0.118586f, -0.172516f, -0.00474148f,
				-0.110252f, 0.06258f, 0.117954f, 0.0963813f, -0.125734f, -0.130778f, 0.0605106f, 0.121093f, -0.0529244f, -0.0252783f,
				-0.0162516f, -0.0249879f, 0.145994f, -0.158315f, 0.146874f, -0.0549978f, -0.0669735f, -0.0636963f, -0.151406f,
				0.0230141f, -0.0123356f, 0.0755378f, -0.0923813f, -0.0947116f, 0.106821f, -0.0626888f, 0.132727f, -0.0142051f,
				-0.154799f, -0.133259f, -0.136168f, -0.0239221f, -0.11901f, 0.0504051f, -0.126613f, -0.0868837f, 0.00281648f,
				-0.0949074f, 0.00500047f, 0.090864f, 0.0114036f, 0.806523f, 1.11244f, 0.561822f, -2.88264f, -0.0444755f, -0.0568296f,
				0.453218f, -1.85186f, 0.551503f, 0.414264f, -0.0828402f, -0.812822f, 0.513695f, 0.725825f, 0.356774f, -0.45647f,
				0.00587389f, 0.072437f, -0.0774712f, -0.119817f, -0.00293383f, -0.128327f, -0.0698329f, -0.145312f, 0.138378f,
				-0.146331f, -0.111355f, -0.12114f, 0.131441f, -0.0913937f, -0.0374071f, 0.0986566f, 0.10964f, 0.0102859f, 0.0511538f,
				0.0641804f, -0.154642f, 0.00595145f, -0.142861f, 0.0131709f, -0.106845f, -0.00351799f, 0.103062f, -0.140484f,
				0.00035955f, 0.066337f, -0.0617432f, 0.052054f, 1.40281f, 0.0107316f, 0.612784f, 1.46941f, 0.0153981f, -3.44994f,
				0.0363773f, -0.251193f, 0.681521f, 0.524972f, -0.127931f, 0.749961f, -1.6537f, -1.54189f, 0.719318f, -0.683289f

			}, 16, 15);

			using OutputLayerBuilder = ValueLayerBuilder<SigmoidLayer<15, 4>>;
			OutputLayerBuilder outputLayerBuilder
			({
				-0.309684f, 0.019991f, 1.24009f, -0.158083f, -1.02288f, -0.273184f, -0.0396015f, 1.11478f, -0.100742f, 0.155708f,
				-0.191399f, -2.64302f, -0.0926339f, 0.182512f, -2.38402f, 0.921295f, 0.685553f, 0.159255f, -0.658508f, -0.175381f,
				-0.810541f, -0.29608f, 0.0732264f, -0.541926f, 0.113515f, -0.0358826f, 0.0774612f, -1.54823f, -0.153562f, 0.0855642f,
				2.42797f, -0.37666f, 1.76547f, 0.253839f, -1.53344f, 1.75848f, 1.30334f, -0.700771f, -0.117457f, 2.30774f, 0.373969f,
				-0.177465f, 0.459159f, 1.02072f, 0.621752f, 0.31464f, 1.87129f, 2.84434f, 0.238606f, 0.0349932f, 1.35674f, 0.694963f,
				2.54577f, -1.80191f, -0.0566268f, -1.60968f, 0.143168f, 0.215891f, 0.378382f, -0.426206f, 0.146716f, 0.124195f,
				0.0996377f, 0.45353f
			}, 16, 4);


			_STD shared_ptr<INeuralNetwork> nn = _STD make_shared<NeuralNetwork>();
			nn->AddLayer(hiddenLayerBuilder);
			nn->AddLayer(outputLayerBuilder);

			_STD shared_ptr<ISample> sample0(
				new Sample({
				1,1,1,
				1,0,1,
				1,0,1,
				1,0,1,
				1,1,1 },
				{ 0,0,0,0 }));
			_STD shared_ptr<ISample> sample1(
				new Sample({
				0,0,1,
				0,0,1,
				0,0,1,
				0,0,1,
				0,0,1 },
				{ 0,0,0,1 }));
			_STD shared_ptr<ISample> sample2(
				new Sample({
				1,1,1,
				0,0,1,
				1,1,1,
				1,0,0,
				1,1,1 },
				{ 0,0,1,0 }));
			_STD shared_ptr<ISample> sample3(
				new Sample({
				1,1,1,
				0,0,1,
				1,1,1,
				0,0,1,
				1,1,1 },
				{ 0,0,1,1 }));
			_STD shared_ptr<ISample> sample4(
				new Sample({
				1,0,1,
				1,0,1,
				1,1,1,
				0,0,1,
				0,0,1 },
				{ 0,1,0,0 }));
			_STD shared_ptr<ISample> sample5(
				new Sample({
				1,1,1,
				1,0,0,
				1,1,1,
				0,0,1,
				1,1,1 },
				{ 0,1,0,1 }));
			_STD shared_ptr<ISample> sample6(
				new Sample({
				1,1,1,
				1,0,0,
				1,1,1,
				1,0,1,
				1,1,1 },
				{ 0,1,1,0 }));
			_STD shared_ptr<ISample> sample7(
				new Sample({
				1,1,1,
				1,0,1,
				0,0,1,
				0,0,1,
				0,0,1 },
				{ 0,1,1,1 }));
			_STD shared_ptr<ISample> sample8(
				new Sample({
				1,1,1,
				1,0,1,
				1,1,1,
				1,0,1,
				1,1,1 },
				{ 1,0,0,0 }));
			_STD shared_ptr<ISample> sample9(
				new Sample({
				1,1,1,
				1,0,1,
				1,1,1,
				0,0,1,
				1,1,1 },
				{ 1,0,0,1 }));

			Trainer t(nn, 0.1f, 0.1f);
			t.AddSample(sample0);
			t.AddSample(sample1);
			t.AddSample(sample2);
			t.AddSample(sample3);
			t.AddSample(sample4);
			t.AddSample(sample5);
			t.AddSample(sample6);
			t.AddSample(sample7);
			t.AddSample(sample8);
			t.AddSample(sample9);

			/*FloatingPoint d = 0;
			for (size_t i = 0; i < 500; i++)
			{
				d = t.Train();
				if (d < 0.01f || _STD isnan(d))
				{
					break;
				}
			}
			if (d > 0.01f || _STD isnan(d))
			{
				Logger::WriteMessage("未能收敛");
				return;
			}*/
			// 测试结果

			auto weights = nn->GetWeights();
			_STD stringstream os("weights:\r\n");
			for (auto& w : weights)
			{
				os << "{ ";
				for (auto val : w)
				{
					os << val << ", ";
				}
				os << "}, " << _STD endl;
			}
			Logger::WriteMessage(os.str().c_str());

			_STD stringstream otest("result:\r\n");
			for (auto& s : t.GetSamples())
			{
				auto& input = s->GetInput();
				for (size_t j = 0; j < input.size(); j++)
				{
					if (j % 3 == 0)
					{
						otest << endl;
					}
					otest << (((int)input[j] == 0) ? "　" : "█");
				}
				otest << endl;
				auto res = nn->Transform(input);
				int resVal = 0;
				for (size_t i = 0; i < 4; i++)
				{
					auto bit = res[i];
					if (bit > 0.9)
					{
						resVal |= 1 << (4 - 1 - i);
					}
				}
				otest << resVal << endl;
			}
			Logger::WriteMessage(otest.str().c_str());
		}
	};
}
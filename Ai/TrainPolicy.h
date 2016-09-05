#pragma once
#include <vector>
#include "Define.h"
#include "ISample.h"
struct DeltaSnapshot
{

};
struct TrainPolicy
{
	template<typename TNeuralType>
	static FloatingPoint Train(const ISample & sample, _STD vector<TNeuralType>& neurals)
	{
		return 0.;
	}
};
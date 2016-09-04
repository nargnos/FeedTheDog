#pragma once
#include <cassert>
#include "Define.h"

class PerceptronDesire
{
public:

	PerceptronDesire(FloatingPoint desire) :
		desire_(desire)
	{

	}

	template<typename TInputArray, typename TWeightArray>
	FloatingPoint Train(
		const TInputArray& input,
		TWeightArray& weights,
		FloatingPoint lastOutput,
		FloatingPoint trainingRate) const
	{
		using namespace std;
		assert(size(input) == size(weights));
		CHECK_ALPHA(trainingRate);
		auto deviation = desire_ - lastOutput;
		transform(begin(input), end(input), begin(weights), begin(weights),
			[deviation = desire_ - lastOutput, trainingRate](auto inputVal, auto weightVal)
		{
			return weightVal + trainingRate * inputVal * deviation;
		});
		return deviation;
	}
protected:
	FloatingPoint desire_;
};

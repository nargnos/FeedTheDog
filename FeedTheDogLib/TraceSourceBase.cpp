#include "stdafx.h"
#include "TraceSourceBase.h"

namespace FeedTheDog
{
	TraceSourceBase::TraceSourceBase()
	{
		openTrace = false;
	}
	void TraceSourceBase::WriteLine(std::string & str, TraceLevel level)
	{
		for each (auto& var in listeners)
		{
			var->WriteLine(str, level);
		}
	}
}  // namespace FeedTheDog


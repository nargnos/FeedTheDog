#pragma once
#include "IListener.h"
namespace FeedTheDog
{
	class TraceSourceBase
	{
	public:
		TraceSourceBase()
		{
			openTrace = false;
		}

	protected:
		bool openTrace;
		_BOOST mutex mutex;

		_STD vector<shared_ptr<IListener>> listeners;
		void WriteLine(_STD string& str, TraceLevel level)
		{
			for each (auto& var in listeners)
			{
				var->WriteLine(str, level);
			}
		}
		
		static const int TimeBufferSize = 64;
		char timeBuffer[TimeBufferSize];
	};
}  // namespace FeedTheDog



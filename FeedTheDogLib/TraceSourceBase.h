#pragma once
#include "IListener.h"
namespace FeedTheDog
{
	class TraceSourceBase
	{
	public:
		TraceSourceBase():
			timeBufferPool(TimeBufferSize)
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
		_BOOST object_pool<_STD ostringstream> stringBuilderPool;
		_BOOST pool<> timeBufferPool;
		static const int TimeBufferSize = 64;
	};
}  // namespace FeedTheDog



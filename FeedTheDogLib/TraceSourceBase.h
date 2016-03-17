#pragma once
#include "IListener.h"
namespace FeedTheDog
{
	class TraceSourceBase
	{
	public:
		TraceSourceBase();

	protected:
		bool openTrace;
		_BOOST mutex mutex;

		_STD vector<shared_ptr<IListener>> listeners;
		void WriteLine(_STD string& str, TraceLevel level);
		static const int TimeBufferSize = 64;
	};
}  // namespace FeedTheDog



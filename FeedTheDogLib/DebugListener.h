#pragma once
#include "ListenerBase.h"
namespace FeedTheDog
{
	class DebugListener :
		public ListenerBase
	{
	public:
		friend ListenerFactory;
	protected:
		DebugListener(Json::Value & listenerConfig);
		virtual void WriteLine(const std::string &) override;

	};

}  // namespace FeedTheDog


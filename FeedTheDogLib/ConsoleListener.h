#pragma once
#include "ListenerBase.h"
namespace FeedTheDog
{
	class ConsoleListener :
		public ListenerBase
	{
	public:
		friend ListenerFactory;
	protected:
		ConsoleListener(Json::Value & listenerConfig);
		virtual void WriteLine(const std::string &) override;
	};

}  // namespace FeedTheDog


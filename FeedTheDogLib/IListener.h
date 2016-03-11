#pragma once
#include "TraceDefine.h"
namespace FeedTheDog
{	
	__interface IListener
	{
		void WriteLine(const _STD string&, TraceLevel);
		void Init(Json::Value& listenerConfig);
		Json::Value & GetDefaultConfig();
	};
}  // namespace FeedTheDog


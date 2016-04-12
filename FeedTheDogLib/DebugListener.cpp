#include "stdafx.h"
#include "DebugListener.h"
namespace FeedTheDog
{
	DebugListener::DebugListener(Json::Value & listenerConfig) :
		ListenerBase(listenerConfig)
	{
	}

	void DebugListener::WriteLine(const std::string & str)
	{
		OutputDebugStringA(str.c_str());
	}
}  // namespace FeedTheDog



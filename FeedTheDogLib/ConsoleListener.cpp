#include "stdafx.h"
#include "ConsoleListener.h"
namespace FeedTheDog
{
	ConsoleListener::ConsoleListener(Json::Value & listenerConfig):
		ListenerBase(listenerConfig)
	{
	}

	void ConsoleListener::WriteLine(const std::string & str)
	{
		_STD cout << str;
	}



}  // namespace FeedTheDog


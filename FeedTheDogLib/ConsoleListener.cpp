#include "stdafx.h"
#include "ConsoleListener.h"
namespace FeedTheDog
{
	ConsoleListener::ConsoleListener()
	{
	}


	ConsoleListener::~ConsoleListener()
	{
	}

	void ConsoleListener::WriteLine(const std::string & str)
	{
		_STD cout << str;
	}


}  // namespace FeedTheDog


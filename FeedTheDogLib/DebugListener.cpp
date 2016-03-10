#include "stdafx.h"
#include "DebugListener.h"
namespace FeedTheDog
{
	DebugListener::DebugListener()
	{
	}


	DebugListener::~DebugListener()
	{
	}
	void DebugListener::WriteLine(const std::string & str)
	{
		OutputDebugStringA(str.c_str());
	}
}  // namespace FeedTheDog



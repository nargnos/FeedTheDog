#include "stdafx.h"
#include "TraceLevel.h"

namespace FeedTheDog
{
	const char* TraceLevel::TraceLevelString[] =
	{
		"trace",
		"debug",
		"info",
		"warning",
		"error",
		"fatal"
	};
	bool TraceLevel::TryParse(string & level, TraceLevel::Level * result)
	{
		for (auto i = static_cast<unsigned char>(Level::_Begin); i < static_cast<unsigned char>(Level::_End); i++)
		{
			if (_BOOST algorithm::equals(level, ConvertToStr(i)))
			{
				if (result != nullptr)
				{
					*result = static_cast<Level>(i);
				}
				return true;
			}
		}
		return false;
	}
	bool TraceLevel::TryParse(TraceLevel::Level level, const char ** result)
	{
		if (CheckLevel(static_cast<unsigned char>(level)))
		{
			if (result != nullptr)
			{
				*result = ConvertToStr(level);
			}
			return true;
		}
		return false;
	}
	const char * TraceLevel::Parse(TraceLevel::Level level)
	{
		const char* result = nullptr;
		TryParse(level, &result);
		return result;
	}
	const char * TraceLevel::ConvertToStr(TraceLevel::Level level)
	{
		return ConvertToStr(static_cast<char>(level));
	}
	bool TraceLevel::CheckLevel(char level)
	{
		return static_cast<unsigned char>(Level::_Begin) <= level && level < static_cast<unsigned char>(Level::_End);
	}
	const char * TraceLevel::ConvertToStr(char level)
	{
		assert(CheckLevel(level));
		return TraceLevelString[level];
	}
}  // namespace FeedTheDog

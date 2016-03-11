#pragma once
namespace FeedTheDog
{
	enum class TraceLevel:char
	{
		_Begin = 0,
		Trace = _Begin,
		Debug,
		Info,
		Warning,
		Error,
		Fatal,
		_End,
	};
	extern const char* TraceLevelString[];
}  // namespace FeedTheDog

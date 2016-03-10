#pragma once
namespace FeedTheDog
{
	enum class TraceLevel
	{
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};
	__interface IListener
	{
		void WriteLine(const _STD string&, TraceLevel);
		void Init(Json::Value& listenerConfig);
	};
}  // namespace FeedTheDog


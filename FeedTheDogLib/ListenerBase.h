#pragma once
#include "IListener.h"
#include "TraceTrait.h"
#include "TraceLevel.h"
namespace FeedTheDog
{
	class ListenerFactory;
	class ListenerBase :
		public TraceSourceTrait::TIListener,
		public _BOOST noncopyable
	{
	public:
		typedef typename TraceSourceTrait::TTraceLevel TTraceLevel;
		typedef typename TTraceLevel::Level TLevel;
		friend ListenerFactory;
		virtual ~ListenerBase();

		virtual void WriteLine(const std::string &str, TLevel level) override;
		static Json::Value GetDefaultConfig();

	protected:
		explicit ListenerBase(Json::Value& listenerConfig);
		virtual void WriteLine(const std::string &) = 0;

		bool visibleLevel[(int)TLevel::_End];
		_BOOST mutex mutex;
		static Json::Value& TraceLevelNode(Json::Value& listenerNode);
	};

}  // namespace FeedTheDog


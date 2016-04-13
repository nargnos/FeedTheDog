#pragma once
#include "IListener.h"
#include "TracePolicy.h"
#include "TraceLevel.h"
namespace FeedTheDog
{
	class ListenerFactory;
	class ListenerBase :
		public TraceSourcePolicy::TIListener,
		public _BOOST noncopyable
	{
	public:
		typedef typename TraceSourcePolicy::TTraceLevel TTraceLevel;
		typedef typename TTraceLevel::Level TLevel;
		friend ListenerFactory;
		virtual ~ListenerBase();

		virtual void WriteLine(const std::string &str, TLevel level) override;
		static Json::Value GetDefaultConfig();

		virtual bool CheckLevel(TLevel level) const override;
	protected:
		explicit ListenerBase(Json::Value& listenerConfig);
		virtual void WriteLine(const std::string &) = 0;

		bool visibleLevel[(int)TLevel::_End];
		_BOOST mutex mutex;
		static Json::Value& TraceLevelNode(Json::Value& listenerNode);

	};

}  // namespace FeedTheDog


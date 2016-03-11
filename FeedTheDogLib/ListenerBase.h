#pragma once
#include "IListener.h"
namespace FeedTheDog
{
	class ListenerBase :
		public IListener
	{
	public:
		ListenerBase();
		~ListenerBase();
		virtual void WriteLine(const std::string &) = 0;
		virtual void WriteLine(const std::string &, TraceLevel) override;
		virtual void Init(Json::Value & listenerConfig) override;
		virtual Json::Value & GetDefaultConfig() override;

	protected:
		bool visibleLevel[(int)TraceLevel::_End];
		_BOOST mutex mutex;
		Json::Value traceLevelStrings;
		Json::Value defaultConfig;

		Json::Value& TraceLevelNode(Json::Value& listenerNode);
		bool CheckTraceLevelConfig(Json::Value& traceLevelNode);
		
	};

}  // namespace FeedTheDog


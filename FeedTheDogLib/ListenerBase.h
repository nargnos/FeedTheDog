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
		// Í¨¹ý IListener ¼Ì³Ð
		virtual void WriteLine(const std::string &, TraceLevel) override;
		virtual void Init(Json::Value & listenerConfig) override;
	private:
		_STD unordered_set<TraceLevel> visibleLevel;
		_BOOST mutex mutex;
		_STD unordered_map<_STD string, TraceLevel> traceLevelStrings;
		Json::Value& TraceLevelNode(Json::Value& listenerNode);
		void CheckTraceLevelConfig(Json::Value& traceLevelNode);
		void DefaultTraceLevelConfig(Json::Value& traceLevelNode);
	};

}  // namespace FeedTheDog


#include "stdafx.h"
#include "ListenerBase.h"

namespace FeedTheDog
{
	ListenerBase::ListenerBase()
	{
		traceLevelStrings.insert(_STD pair<_STD string, TraceLevel >("trace", TraceLevel::Trace));
		traceLevelStrings.insert(_STD pair<_STD string, TraceLevel >("debug", TraceLevel::Debug));
		traceLevelStrings.insert(_STD pair<_STD string, TraceLevel >("info", TraceLevel::Info));
		traceLevelStrings.insert(_STD pair<_STD string, TraceLevel >("warning", TraceLevel::Warning));
		traceLevelStrings.insert(_STD pair<_STD string, TraceLevel >("error", TraceLevel::Error));
		traceLevelStrings.insert(_STD pair<_STD string, TraceLevel >("fatal", TraceLevel::Fatal));
	}


	ListenerBase::~ListenerBase()
	{
	}
	void ListenerBase::WriteLine(const std::string &str, TraceLevel level)
	{
		if (visibleLevel.count(level) <= 0)
		{
			return;
		}
		mutex.lock();
		WriteLine(str);
		mutex.unlock();
	}
	
	void ListenerBase::Init(Json::Value & listenerConfig)
	{
		assert(!listenerConfig);
		// 设置过滤
		auto& traceLevelNode = TraceLevelNode(listenerConfig);
		// 只要是错误设置都会恢复成默认值（包括大小写错误）
		CheckTraceLevelConfig(traceLevelNode);
		for each (auto& var in traceLevelNode)
		{
			assert(var.isString());

			auto& tmpStr = var.asString();
			assert(traceLevelStrings.count(tmpStr));
			visibleLevel.insert(traceLevelStrings[tmpStr]);

		}
	}
	Json::Value & ListenerBase::TraceLevelNode(Json::Value & listenerNode)
	{
		return listenerNode["TraceLevel"];
	}
	void ListenerBase::CheckTraceLevelConfig(Json::Value & traceLevelNode)
	{
		if (!traceLevelNode.isArray())
		{
			DefaultTraceLevelConfig(traceLevelNode);
		}
		else
		{
			bool hasError = false;
			for each (auto& var in traceLevelNode)
			{
				if (!var.isString() ||
					!traceLevelStrings.count(var.asString()))
				{
					hasError = true;
					break;
				}
			}
			if (hasError)
			{
				DefaultTraceLevelConfig(traceLevelNode);
			}
		}
	}
	void ListenerBase::DefaultTraceLevelConfig(Json::Value & traceLevelNode)
	{
		traceLevelNode.clear();
		for each (auto& var in traceLevelStrings)
		{
			traceLevelNode.append(var.first);
		}
	}
}  // namespace FeedTheDog


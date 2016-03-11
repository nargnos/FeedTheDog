#include "stdafx.h"
#include "ListenerBase.h"
#include "TraceDefine.h"
namespace FeedTheDog
{
	ListenerBase::ListenerBase()
	{
		auto& traceLevelNode = TraceLevelNode(defaultConfig);
		for (auto i = (int)TraceLevel::_Begin; i < (int)TraceLevel::_End; i++)
		{
			// ���ù���
			visibleLevel[i] = false;
			// ����Ĭ�ϸ��ٵȼ�
			traceLevelNode.append(TraceLevelString[i]);
			// ����ת����
			traceLevelStrings[TraceLevelString[i]] = i;
		}
	}


	ListenerBase::~ListenerBase()
	{
	}
	void ListenerBase::WriteLine(const _STD string &str, TraceLevel level)
	{
		if (!visibleLevel[(int)level])
		{
			return;
		}
		mutex.lock();
		WriteLine(str);
		mutex.unlock();
	}

	void ListenerBase::Init(Json::Value & listenerConfig)
	{
		assert(!listenerConfig.empty());
		// ���ù���
		auto& traceLevelNode = TraceLevelNode(listenerConfig);
		// ֻҪ�Ǵ������ö���ָ���Ĭ��ֵ��������Сд����
		if(!CheckTraceLevelConfig(traceLevelNode))
		{
			traceLevelNode = TraceLevelNode(defaultConfig);
		}
		for each (auto& var in traceLevelNode)
		{
			assert(var.isString());

			auto& tmpStr = var.asString();
			auto& traceLevelStr = traceLevelStrings[tmpStr];
			assert(!traceLevelStr.empty());
			assert(traceLevelStr.isInt());
			int index = traceLevelStr.asInt();
			if (index < (int)TraceLevel::_End && index >= (int)TraceLevel::_Begin)
			{
				visibleLevel[index] = true;
			}
		}
	}
	Json::Value & ListenerBase::TraceLevelNode(Json::Value & listenerNode)
	{
		return listenerNode["TraceLevel"];
	}
	bool ListenerBase::CheckTraceLevelConfig(Json::Value & traceLevelNode)
	{
		if (!traceLevelNode.isArray())
		{
			return false;
		}
		else
		{
			bool hasError = false;
			for each (auto& var in traceLevelNode)
			{
				if (!var.isString() ||
					traceLevelStrings[var.asString()].empty())
				{
					hasError = true;
					break;
				}
			}
			return !hasError;
		}
	}
	Json::Value & ListenerBase::GetDefaultConfig()
	{
		return defaultConfig;		
	}
}  // namespace FeedTheDog


#include "stdafx.h"
#include "ListenerBase.h"
namespace FeedTheDog
{
	ListenerBase::~ListenerBase()
	{
	}
	void ListenerBase::WriteLine(const std::string & str, TLevel level)
	{
		if (!CheckLevel(level))
		{
			return;
		}
		mutex.lock();
		WriteLine(str);
		mutex.unlock();
	}
	Json::Value ListenerBase::GetDefaultConfig()
	{
		Json::Value result;
		auto& defaultTraceLevelNode = TraceLevelNode(result);
		for (auto i = (int)TLevel::_Begin; i < (int)TLevel::_End; i++)
		{
			// ����Ĭ�ϸ��ٵȼ�
			defaultTraceLevelNode.append(TTraceLevel::ConvertToStr(i));
		}


		return result;
	}
	ListenerBase::ListenerBase(Json::Value & listenerConfig)
	{
		// ��ʼ��Ĭ������

		for (auto i = (int)TLevel::_Begin; i < (int)TLevel::_End; i++)
		{
			// ���ù���
			visibleLevel[i] = false;
		}


		assert(!listenerConfig.empty());
		// ���ù���
		auto& traceLevelNode = TraceLevelNode(listenerConfig);

		if (!traceLevelNode.isArray() || traceLevelNode.empty())
		{
			traceLevelNode.clear();
		}

		for each (auto& var in traceLevelNode)
		{
			if (var.isString())
			{
				string tmpStr = var.asString();
				_BOOST algorithm::to_lower(tmpStr);
				TLevel level;
				if (TTraceLevel::TryParse(tmpStr, &level))
				{
					visibleLevel[static_cast<int>(level)] = true;
				}
			}
		}

	}
	Json::Value & ListenerBase::TraceLevelNode(Json::Value & listenerNode)
	{
		auto& result = listenerNode["TraceLevel"];
		if (!result.isArray())
		{
			result.clear();
		}
		return result;
	}
	bool ListenerBase::CheckLevel(ListenerBase::TLevel level) const
	{
		return visibleLevel[(int)level];
	}
}  // namespace FeedTheDog


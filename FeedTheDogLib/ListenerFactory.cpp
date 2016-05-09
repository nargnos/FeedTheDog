#include "stdafx.h"
#include "ListenerFactory.h"
namespace FeedTheDog
{
	

	ListenerFactory::TListenerVector ListenerFactory::CreateListeners(Json::Value & listenerConfig, bool isBuildAll)
	{
		// ��ʼ����
		TListenerVector result;

		for each (auto& var in listenerRegister)
		{
			auto& tmp = listenerConfig[var.first.c_str()];
			if (isBuildAll || !tmp.empty())
			{
				result.push_back(var.second.first(tmp));
			}
		}
		return result;
	}

	const ListenerFactory::TRegsterType & ListenerFactory::GetListenerRegister()
	{
		return listenerRegister;
	}

	// TODO: ������Ըĳ�������dll�ķ�ʽ������
	ListenerFactory::TRegsterType ListenerFactory::listenerRegister
	{
		{ "Console", {ListenerFactory::Create<ConsoleListener>,ConsoleListener::GetDefaultConfig} },
		{ "File", {ListenerFactory::Create<FileListener> ,FileListener::GetDefaultConfig }},
		{ "Debug", {ListenerFactory::Create<DebugListener> ,DebugListener::GetDefaultConfig }}
	};
}  // namespace FeedTheDog




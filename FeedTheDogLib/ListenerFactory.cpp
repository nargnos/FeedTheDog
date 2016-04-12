#include "stdafx.h"
#include "ListenerFactory.h"
namespace FeedTheDog
{
	ListenerFactory::ListenerFactory()
	{
	}


	ListenerFactory::TListenerVector ListenerFactory::CreateListeners(Json::Value & listenerConfig, bool isBuildAll)
	{
		// 开始构建
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

	ListenerFactory::~ListenerFactory()
	{
	}
	ListenerFactory::TRegsterType ListenerFactory::listenerRegister
	{
		{ "Console", {ListenerFactory::Create<ConsoleListener>,ConsoleListener::GetDefaultConfig} },
		{ "File", {ListenerFactory::Create<FileListener> ,FileListener::GetDefaultConfig }},
		{ "Debug", {ListenerFactory::Create<DebugListener> ,DebugListener::GetDefaultConfig }}
	};
}  // namespace FeedTheDog




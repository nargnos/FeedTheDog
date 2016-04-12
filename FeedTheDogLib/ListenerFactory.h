#pragma once
#include "TraceTrait.h"
#include "ITraceConfig.h"
#include "FileListener.h"
#include "DebugListener.h"
#include "ConsoleListener.h"
namespace FeedTheDog
{
	class ListenerFactory
	{
	public:
		typedef typename TraceSourceTrait::TTraceConfig::TListenerVector TListenerVector;
		typedef _STD map<string,
			_STD pair<
			_STD function<unique_ptr<TraceSourceTrait::TIListener>(Json::Value &)>, // 创建函数
			_STD function<Json::Value()> // 默认设置函数
			>
		> TRegsterType;

		static TListenerVector CreateListeners(Json::Value & listenerConfig, bool isBuildAll = false);
		static const TRegsterType& GetListenerRegister();
		~ListenerFactory();
	private:
		ListenerFactory();
		template<typename TListener>
		static unique_ptr<TraceSourceTrait::TIListener> Create(Json::Value & listenerConfig)
		{
			// listenerConfig为empty也会用默认设置构造
			return unique_ptr<TraceSourceTrait::TIListener>(new TListener(listenerConfig));
		}
		// 注册在此处
		static TRegsterType listenerRegister;
	};


}  // namespace FeedTheDog

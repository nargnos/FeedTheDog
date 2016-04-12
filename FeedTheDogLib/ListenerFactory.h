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
			_STD function<unique_ptr<TraceSourceTrait::TIListener>(Json::Value &)>, // ��������
			_STD function<Json::Value()> // Ĭ�����ú���
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
			// listenerConfigΪemptyҲ����Ĭ�����ù���
			return unique_ptr<TraceSourceTrait::TIListener>(new TListener(listenerConfig));
		}
		// ע���ڴ˴�
		static TRegsterType listenerRegister;
	};


}  // namespace FeedTheDog

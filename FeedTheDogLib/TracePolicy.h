#pragma once
#include "TraceLevel.h"
#include "ITraceConfig.h"
#include "IListener.h"
namespace FeedTheDog
{
	template<typename TTraceLevel>
	struct IListener;
	template<typename TTraceLevel>
	class TraceFormat;

	struct TraceSourcePolicy
	{
		typedef TraceLevel TTraceLevel;
		typedef typename TTraceLevel::Level TLevel;
		typedef IListener<TLevel> TIListener;
		typedef ITraceConfig<TIListener> TTraceConfig;
		typedef typename TTraceConfig::TListenerVector TListenerVector;
		typedef TraceFormat<TTraceLevel> TTraceFormat;


		static void WriteLine(TListenerVector& listeners, string & str, TLevel level)
		{
			for each (auto& var in listeners)
			{
				var->WriteLine(str, level);
			}
		}
	};
}  // namespace FeedTheDog

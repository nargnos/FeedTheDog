#pragma once
#include "TraceLevel.h"
namespace FeedTheDog
{
	template<typename TIListener>
	struct ITraceConfig;
	template<typename TTraceLevel>
	struct IListener;
	template<typename TTraceLevel>
	class TraceFormat;

	struct TraceSourcePolicy
	{		
		typedef TraceLevel TTraceLevel;
		typedef IListener<typename TTraceLevel::Level> TIListener;
		typedef ITraceConfig<TIListener> TTraceConfig;
		typedef TraceFormat<TTraceLevel> TTraceFormat;
	};
}  // namespace FeedTheDog

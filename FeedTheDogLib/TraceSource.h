#pragma once
#include "TraceSourceImpl.h"
#include "TraceTrait.h"
#include "TraceLevel.h"
#include "ITraceConfig.h"
#include "IListener.h"
#include "TraceFormat.h"

namespace FeedTheDog
{
	typedef TraceSourceImpl<TraceSourceTrait> TraceSource;
}  // namespace FeedTheDog

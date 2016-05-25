#pragma once
#include "TraceSourceImpl.h"
#include "TracePolicy.h"
#include "TraceLevel.h"
#include "ITraceConfig.h"
#include "IListener.h"
#include "TraceFormat.h"

namespace FeedTheDog
{
	using TraceSource = TraceSourceImpl<TraceSourcePolicy>;
}  // namespace FeedTheDog

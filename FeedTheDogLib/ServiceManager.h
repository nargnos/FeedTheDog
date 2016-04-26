#pragma once
#include "stdafx.h"
#include "Config.h"
#include "Define.h"
#include "CorePolicy.h"
#include "IService.h"
#include "Worker.h"
#include "WorkerPool.h"
#include "SessionPool.h"
#include "Core.h"

namespace FeedTheDog
{
	typedef Core<CorePolicy> ServiceManager;

}  // namespace FeedTheDog

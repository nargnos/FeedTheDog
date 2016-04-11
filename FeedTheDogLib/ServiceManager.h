#pragma once
#include "stdafx.h"
#include "Config.h"
#include "Trait.h"
#include "CoreTrait.h"
#include "IService.h"
#include "Worker.h"
#include "SessionPool.h"
#include "Core.h"

namespace FeedTheDog
{
	typedef Core<CoreTrait> ServiceManager;

}  // namespace FeedTheDog

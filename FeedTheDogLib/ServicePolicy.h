#pragma once
#include "ServiceManager.h"
#include "SessionManager.h"
namespace FeedTheDog
{
	struct ServicePolicy
	{
		typedef ServiceManager TServiceManager;
		typedef SessionManager TSessionManager;

	};
}  // namespace FeedTheDog

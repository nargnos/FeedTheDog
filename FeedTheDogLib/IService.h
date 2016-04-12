#pragma once
#include "Define.h"
#include "CorePolicy.h"

namespace FeedTheDog
{
	template<typename TServiceManager>
	struct IService
	{
		typedef TServiceManager TServiceManager;
		virtual const char* Name() const = 0;
		virtual void AsyncStart() = 0;
		virtual void Stop() = 0;
		virtual bool Init(TServiceManager*) = 0;
	};
}  // namespace FeedTheDog

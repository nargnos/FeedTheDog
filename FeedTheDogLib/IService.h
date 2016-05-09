#pragma once
#include "Define.h"
#include "IAddon.h"
namespace FeedTheDog
{
	template<typename TServiceManager>
	struct IService
	{
		virtual const char* Name() const = 0;
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual bool Init(TServiceManager*) = 0;
		virtual bool AddAddon(shared_ptr<IAddon>&) = 0;
		virtual void RemoveAddon(const char*) = 0;
		virtual ~IService() = default;
	};
}  // namespace FeedTheDog

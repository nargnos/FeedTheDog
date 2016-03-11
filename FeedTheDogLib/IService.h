#pragma once
#include "Trait.h"
#include "CoreTrait.h"

namespace FeedTheDog
{
	struct IService
	{
		typedef typename CoreTrait::TCore TCore;
		typedef IService TService;
		virtual const char* Name() const = 0;
		virtual void AsyncStart() = 0;
		virtual void Stop() = 0;
		virtual bool Init(TCore*) = 0;
	};
}  // namespace FeedTheDog

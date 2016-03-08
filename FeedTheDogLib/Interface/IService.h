#pragma once
#include "..\Common.h"
#include "..\Trait\Trait.h"
#include "..\Trait\CoreTrait.h"

namespace FeedTheDog
{
	struct IService
	{
		typedef typename CoreTrait::TCore TCore;
		typedef IService TService;
		virtual int UniqueID() const = 0;
		virtual void AsyncStart() = 0;
		virtual void Stop() = 0;
		virtual bool Init(TCore* core) = 0;
	};
}  // namespace FeedTheDog

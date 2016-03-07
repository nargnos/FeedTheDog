#pragma once
#include "..\Trait\Trait.h"
#include "..\Trait\CoreTrait.h"

namespace FeedTheDog
{
	struct IService
	{
		typedef typename CoreTrait::TCore TCore;
		typedef IService TService;
		virtual int UniqueID() const = 0;
		virtual void Run(TCore* core) = 0;		
	};
}  // namespace FeedTheDog

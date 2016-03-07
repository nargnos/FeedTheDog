#pragma once
#include "..\Interface\IService.h"
#include "..\Core\Core.h"
#include "..\Core\Worker.h"
#include "..\Pool\PoolElement\Session.h"
#include "..\Pool\SessionPool.h"

namespace FeedTheDog
{
	template<int TUniqueID>
	struct ServiceBase:
		public IService
	{
		virtual int UniqueID() const override
		{
			return TUniqueID;
		}
		virtual ~ServiceBase() = default;
	};

}  // namespace FeedTheDog

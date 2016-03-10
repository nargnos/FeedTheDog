#pragma once
#include "IService.h"
#include "Core.h"
#include "Worker.h"
#include "Session.h"
#include "SessionPool.h"

namespace FeedTheDog
{
	template<long long TUniqueID>
	struct ServiceBase :
		public IService
	{
		static const long long ID = TUniqueID;
		virtual long long UniqueID() const override
		{
			return TUniqueID;
		}
		virtual ~ServiceBase() = default;
	};

}  // namespace FeedTheDog

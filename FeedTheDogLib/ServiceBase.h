#pragma once
#include "IService.h"
#include "Core.h"
#include "Worker.h"
#include "Session.h"
#include "SessionPool.h"

namespace FeedTheDog
{
	struct ServiceBase :
		public IService
	{
		ServiceBase(const char* name)
		{
			name_ = name;
		}
		virtual const char* Name() const override
		{
			return name_;
		}
		virtual ~ServiceBase() = default;
	protected:
		const char* name_;
	};

}  // namespace FeedTheDog

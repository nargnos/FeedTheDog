#include "stdafx.h"
#include "ServiceBase.h"

namespace FeedTheDog
{
	ServiceBase::ServiceBase(const char * name) :
		name_(name)
		
	{
	}
	ServiceBase::~ServiceBase()
	{
		
	}
	const char * ServiceBase::Name() const
	{
		return name_;
	}
	bool ServiceBase::AddAddon(shared_ptr<IAddon>& addon)
	{
		return true;
	}
	void ServiceBase::RemoveAddon(const char * ptr)
	{
		
	}

	const ServiceBase::ImplPtr& ServiceBase::Impl()const
	{
		assert(impl_);
		return impl_;
	}

	bool ServiceBase::Init(const shared_ptr<ServiceBaseImpl>& impl)
	{
		assert(!impl_);
		impl_ = impl;
		return Init();
	}
	
}  // namespace FeedTheDog

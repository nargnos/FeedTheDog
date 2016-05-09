#include "stdafx.h"
#include "ServiceBase.h"

namespace FeedTheDog
{
	ServiceBase::ServiceBase(const char * name) :
		impl_(make_unique<ServiceBaseImpl>(name))
	{
	}
	ServiceBase::~ServiceBase()
	{
	}
	const char * ServiceBase::Name() const
	{
		return impl_->Name();
	}
	bool ServiceBase::Init(TServiceManager * managerPtr)
	{
		impl_->Init(managerPtr);
		return InitService();
	}
	bool ServiceBase::AddAddon(shared_ptr<IAddon>& addon)
	{
		return impl_->AddAddon(addon);
	}
	void ServiceBase::RemoveAddon(const char * ptr)
	{
		return impl_->RemoveAddon(ptr);
	}
	ServiceBase::TWorker * ServiceBase::SelectIdleWorker()
	{
		return impl_->SelectIdleWorker();
	}
	const unique_ptr<ServiceBase::TTraceSource>& ServiceBase::GetTrace() const
	{
		return impl_->GetTrace();
	}
	ServiceBase::TWorker * ServiceBase::SelectWorker()
	{
		return impl_->SelectWorker();
	}
	
}  // namespace FeedTheDog

#pragma once
#include "Config.h"
#include "Define.h"
#include "IService.h"
#include "Worker.h"
#include "WorkerPool.h"
#include "ServiceManager.h"
#include "ServiceBaseImpl.h"

namespace FeedTheDog
{
	// TODO: session连接需要加密或者代理等扩展，要么做到addon里，要么做成新service，post到新的处理

	class ServiceBase :
		public IService,
		public _BOOST noncopyable,
		protected ServiceDefines
	{
	public:

		using ImplPtr = shared_ptr<ServiceBaseImpl>;
		explicit ServiceBase(const char* name);

		virtual ~ServiceBase();

		virtual const char * Name() const override;

		virtual bool Init(const shared_ptr<ServiceBaseImpl>&) override;
		virtual bool AddAddon(shared_ptr<IAddon>& addon) override;
		virtual void RemoveAddon(const char * ptr) override;
	protected:
		virtual bool Init() = 0;
		const ImplPtr& Impl() const;

	private:

		ImplPtr impl_;
		const char* name_;

	};


}  // namespace FeedTheDog

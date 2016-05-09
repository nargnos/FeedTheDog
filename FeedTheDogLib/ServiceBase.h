#pragma once
#include "ServiceBaseImpl.h"
namespace FeedTheDog
{

	// TODO: session连接需要加密或者代理等扩展，要么做到addon里，要么做成新service，post到新的处理
	class ServiceBase :
		public IService<ServiceDefines::TServiceManager>,
		public _BOOST noncopyable,
		public ServiceDefines
	{
	public:
	
		ServiceBase(const char* name);

		~ServiceBase();

		virtual const char * Name() const override;
		virtual bool Init(TServiceManager * managerPtr) override;
		virtual bool AddAddon(shared_ptr<IAddon>& addon) override;
		virtual void RemoveAddon(const char * ptr) override;
	protected:
		virtual bool InitService() = 0;
		ServiceBase::TWorker* FASTCALL SelectIdleWorker();
		const unique_ptr<ServiceBase::TTraceSource>& FASTCALL GetTrace() const;
		ServiceBase::TWorker* FASTCALL SelectWorker();

		template<typename TProtocol, bool hasBuffer = true>
		inline shared_ptr<TSession<TProtocol, hasBuffer>> FASTCALL NewSession()
		{
			return impl_->NewSession<TProtocol, hasBuffer>();
		}
		template<typename TProtocol>
		inline const _STD unique_ptr<TResolver<TProtocol>>& FASTCALL GetResolver() const
		{
			return impl_->GetResolver<TProtocol>();
		}

	private:
		using ImplPtr = unique_ptr<ServiceBaseImpl>;
		ImplPtr impl_;
	};


}  // namespace FeedTheDog

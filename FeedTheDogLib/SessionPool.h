#pragma once
#include "SessionPoolImpl.h"
namespace FeedTheDog
{
	struct CheckAlloc
	{
		static void Check(void* ptr)
		{
			if (ptr == nullptr)
			{
				throw _STD exception("alloc failed");
			}
		}
	};
	template<typename TSession>
	class SessionPool:
		public _BOOST noncopyable
	{
	public:
		using TPool = SessionPoolImpl<TSession>;
		typedef TSession TSession;
		// using AlignedDelete = _BOOST alignment::aligned_delete;

		// using TPoolPtr = unique_ptr<TPool, AlignedDelete>;

		SessionPool(_ASIO io_service& ios, size_t poolSize, _STD atomic<size_t>& counter) :
			impl_(CreateSessionPool(ios, poolSize, counter))
		{
		}
		inline shared_ptr<TSession> FASTCALL NewSession()
		{
			assert(impl_ != nullptr);
			return impl_->NewSession();
		}
		/*void CloseAll()
		{
			assert(impl_ != nullptr);		
			impl_->CloseAll();
		}*/
		~SessionPool()
		{
			assert(impl_ != nullptr);
			impl_->~SessionPoolImpl();
			_BOOST alignment::aligned_free(impl_);
			impl_ = nullptr;
		}
	protected:

		static TPool* CreateSessionPool(_ASIO io_service& ios, size_t poolSize, _STD atomic<size_t>& counter)
		{
			auto pos = _BOOST alignment::aligned_alloc(ALIGNSIZE, sizeof(TPool));
			CheckAlloc::Check(pos);
			new (pos) TPool(ios, poolSize, counter);

			return reinterpret_cast<TPool*>(pos);
		}
		TPool* impl_;
	};

}  // namespace FeedTheDog
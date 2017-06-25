#ifndef TIMERCALLBACK_H_
#define TIMERCALLBACK_H_
#include <memory>
#include "Noncopyable.h"
namespace Detail
{

	class TimerCallback :
		public std::enable_shared_from_this<TimerCallback>,
		public Noncopyable
	{
	public:
		using Cb = void(*)(const TimerCallback*);
		explicit TimerCallback(Cb cb);
		void Invoke() const;
		virtual ~TimerCallback() = default;
	private:
		Cb cb_;
	};
	template<typename THandler>
	class TimerCallbackImpl :
		public TimerCallback
	{
	public:
		// void(const TimerCallback&)
		explicit TimerCallbackImpl(THandler&& handler) :
			TimerCallback(TimerCallbackImpl::Invoke),
			handler_(std::forward<THandler>(handler))
		{
			static_assert(std::is_assignable<std::function<void(const TimerCallback&)>, THandler>::value, "callback error");
		}
	private:
		static void Invoke(const TimerCallback* ptr)
		{
			auto self = static_cast<const TimerCallbackImpl<THandler>*>(ptr);
			self->handler_(*ptr);
		}
		THandler handler_;
	};

	using TimerPtr = std::shared_ptr<const TimerCallback>;
	// void(const TimerCallback&)
	template<typename THandler>
	TimerPtr MakeTimerCallback(THandler&& cb)
	{
		return TimerPtr(new TimerCallbackImpl<THandler>(std::forward<THandler>(cb)));
	}
}  // namespace Detail
using Detail::MakeTimerCallback;
using Detail::TimerPtr;
#endif // TIMERCALLBACK_H_


#ifndef TIMEHEAP_H_
#define TIMEHEAP_H_
#include <memory>
#include <functional>
#include <chrono>
#include <queue>
#include "TimerCallback.h"
#include "Noncopyable.h"
namespace Detail
{
	// TODO: 自己用一个fd维护好呢还是创建一堆fd由系统处理好呢？
	// 由系统处理的好写些，但会有大量fd
	// 自己维护的不好写interval，只能靠触发时再次添加来解决
	// epoll_wait 的超时设置也可以用来做，不过要改结构就算了
	using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
	struct TimeHeapNode
	{
		TimerPtr Ptr;
		TimePoint Deadline;
		friend bool operator>(const TimeHeapNode& l, const TimeHeapNode& r)
		{
			return l.Deadline > r.Deadline;
		}
	};
	// 非线程安全
	class TimeHeap:
		public Noncopyable
	{
	public:
		using Heap = std::priority_queue<TimeHeapNode, std::vector<TimeHeapNode>, std::greater<TimeHeapNode>>;

		// 执行, 返回下次触发间隔ms,无触发返回0
		long long Do();
		// 添加，多次添加就只按最后一次的时间触发
		// 返回规则同Do
		template<typename T>
		long long Add(TimerPtr&& ptr, T&& duration)
		{
			heap_.push({ std::move(ptr),std::chrono::system_clock::now() + duration });
			return Do();
		}
	private:
		static long long GetMsDuration(TimePoint l, TimePoint r);
		Heap heap_;

	};
}  // namespace Detail
using Detail::TimerCallback;
#endif // TIMEHEAP_H_


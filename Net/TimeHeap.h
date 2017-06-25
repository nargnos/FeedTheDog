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
	// TODO: �Լ���һ��fdά�����ػ��Ǵ���һ��fd��ϵͳ������أ�
	// ��ϵͳ����ĺ�дЩ�������д���fd
	// �Լ�ά���Ĳ���дinterval��ֻ�ܿ�����ʱ�ٴ���������
	// epoll_wait �ĳ�ʱ����Ҳ����������������Ҫ�Ľṹ������
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
	// ���̰߳�ȫ
	class TimeHeap:
		public Noncopyable
	{
	public:
		using Heap = std::priority_queue<TimeHeapNode, std::vector<TimeHeapNode>, std::greater<TimeHeapNode>>;

		// ִ��, �����´δ������ms,�޴�������0
		long long Do();
		// ��ӣ������Ӿ�ֻ�����һ�ε�ʱ�䴥��
		// ���ع���ͬDo
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


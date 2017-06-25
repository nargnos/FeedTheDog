#include "TimeHeap.h"



Detail::TimerCallback::TimerCallback(Cb cb) :
	cb_(cb)
{
}

void Detail::TimerCallback::Invoke() const
{
	cb_(this);
}

long long Detail::TimeHeap::Do()
{
	auto now = std::chrono::system_clock::now();
	while (!heap_.empty())
	{
		auto& node = heap_.top();
		auto ms = GetMsDuration(node.Deadline, now);
		if (ms > 0)
		{
			// ʱ��δ��
			return ms;
		}
		// ����ʱ��������unique�ͻ���ã���ʱ����������������ã��Ͳ������
		if (node.Ptr.unique())
		{
			auto ptr = node.Ptr;
			heap_.pop();
			ptr->Invoke();
		}
		else
		{
			heap_.pop();
		}

	}
	return 0;
}

long long Detail::TimeHeap::GetMsDuration(TimePoint l, TimePoint r)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(l - r).count();
}

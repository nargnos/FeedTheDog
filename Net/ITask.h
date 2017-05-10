#ifndef ITASK_H_
#define ITASK_H_
#include <memory>
namespace Detail
{
	class Loop;
	class ITask
	{
	public:
		ITask() = default;
		virtual ~ITask() = default;
		// ִ����Ϸ���true�����򷵻�false�ȴ��´�ִ��
		virtual bool DoEvent(Loop& loop) = 0;
	};

}  // namespace Detail

#endif // ITASK_H_


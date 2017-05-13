#ifndef REGISTERTASKATTORNEY_H_
#define REGISTERTASKATTORNEY_H_
#include <memory>
#include "ITask.h"
namespace Detail
{
	class Loop;
	class RegisterTaskAttorney
	{
	public:
		static void RegisterTask(Loop& loop, std::shared_ptr<ITask>&& ptr);
	private:
		RegisterTaskAttorney() = delete;
		~RegisterTaskAttorney() = delete;
	};

}  // namespace Detail

#endif // REGISTERTASKATTORNEY_H_


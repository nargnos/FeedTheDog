#ifndef REGISTERTASKATTORNEY_H_
#define REGISTERTASKATTORNEY_H_
#include <memory>
#include "ITask.h"
class Loop;
class RegisterTaskAttorney
{
public:
	static void RegisterTask(Loop& loop, std::shared_ptr<ITask>&& ptr);
private:
	RegisterTaskAttorney() = delete;
	~RegisterTaskAttorney() = delete;
};


#endif // REGISTERTASKATTORNEY_H_


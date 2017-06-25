#include "Task.h"
#include "RegisterTaskAttorney.h"
void Detail::PostTask(Loop & loop, ITaskPtr && task)
{
	RegisterTaskAttorney::RegisterTask(loop, std::move(task));
}

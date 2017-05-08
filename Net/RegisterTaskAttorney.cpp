#include "RegisterTaskAttorney.h"
#include "Loop.h"
void RegisterTaskAttorney::RegisterTask(Loop & loop, std::shared_ptr<ITask>&& ptr)
{
	loop.RegisterTask(std::move(ptr));
}

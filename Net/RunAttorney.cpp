#include "RunAttorney.h"
#include "Loop.h"
namespace Detail
{
	void RunAttorney::Start(Loop & loop)
	{
		loop.Start();
	}

	void RunAttorney::Stop(Loop & loop)
	{
		loop.Stop();
	}

	void RunAttorney::PrepareBuffers(Loop & loop)
	{
		loop.PrepareBuffers();
	}

}  // namespace Detail
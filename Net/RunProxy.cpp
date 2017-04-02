#include "RunProxy.h"
#include "Loop.h"
void RunProxy::Start(Loop & loop)
{
	loop.Start();
}

void RunProxy::Stop(Loop & loop)
{
	loop.Stop();
}

void RunProxy::PrepareBuffers(Loop & loop)
{
	loop.PrepareBuffers();
}

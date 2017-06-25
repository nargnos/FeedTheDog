#include "TlsPackage.h"
#include "FDTaskCtlAttorney.h"
Detail::GlobalTlsPackage & Detail::GlobalTlsPackage::Instance()
{
	thread_local static GlobalTlsPackage pkg;
	return pkg;
}

void Detail::GlobalTlsPackage::SetLoop(Loop * loop)
{
	assert(TlsLoop == nullptr);
	*const_cast<Loop**>(&TlsLoop) = loop;
	FDTaskCtlAttorney::Add(*loop, EPOLLIN, &TlsTimerManager);
}

Detail::GlobalTlsPackage::GlobalTlsPackage() :
	TlsLoop(nullptr)
{

}



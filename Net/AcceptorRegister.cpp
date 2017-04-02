#include "AcceptorRegister.h"
#include "IoService.h"
#include "ITcpAcceptor.h"
#include "FDTaskCtlProxy.h"
void AcceptorRegister::Register(IoService & ios, ITcpAcceptor & act)
{
	// LT ģʽ��ӣ���ֹ©��
	FDTaskCtlProxy::Add(*ios.loop_, EPOLLIN, &act);
}

void AcceptorRegister::UnRegister(IoService & ios, ITcpAcceptor & act)
{
	FDTaskCtlProxy::Del(*ios.loop_, &act);
}

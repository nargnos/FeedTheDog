#include "ProactorConnection.h"
namespace Detail
{

	bool ProactorConnectionBase::IsClosed() const
	{
		return readState_.IsClose();
	}

	ProactorConnectionBase::ProactorConnectionBase(Loop & loop, IoState::IoStatus readStat, IoState::IoStatus writeStat) :
		loop_(loop),
		readState_(readStat),
		writeState_(writeStat),
		isSocketRegistered_(false),
		isTaskRegistered_(false)
	{
	}

	bool ProactorConnectionBase::HasError() const
	{
		return readState_.HasError();
	}

	bool ProactorConnectionBase::IsGood() const
	{
		// ����״̬��ͬʱ���ã���������ֻ���ж�һ��
		return readState_.IsGood();
		//return readState_.IsGood() && writeState_.IsGood();
	}

	Loop & ProactorConnectionBase::GetLoop() const
	{
		return loop_;
	}


	bool ProactorConnectionBase::OnBuffReady(IoState & s)
	{
		s.SetBuffReady(true);
		return s.IsDoIo();
	}

	bool ProactorConnectionBase::OnIoReady(IoState & s)
	{
		s.SetIoReady(true);
		return s.IsDoIo();
	}

	void ProactorConnectionBase::OnFailState() const
	{
		assert(false);
		throw nullptr;
	}

	IoState & ProactorConnectionBase::ReadState()
	{
		return readState_;
	}

	IoState & ProactorConnectionBase::WriteState()
	{
		return writeState_;
	}

	// io�����е���, ���������ע��epoll�ȴ�io�ɴ���

	void ProactorConnectionBase::OnCantIo(IoState & s)
	{
		assert(s.IsDoIo());
		s.SetIoReady(false);
		assert(!s.IsIoReady());
	}

	// �¼����ʱ����

	void ProactorConnectionBase::OnNoBuff(IoState & s)
	{
		s.SetBuffReady(false);
	}

	void ProactorConnectionBase::UnregisterSocket()
	{
		if (!isSocketRegistered_.load(std::memory_order_acquire))
		{
			return;
		}
		FDTaskCtlAttorney::Del(loop_, this);
		isSocketRegistered_.store(false,std::memory_order_release);
	}

	void ProactorConnectionBase::RegisterSocket()
	{
		// FIX: ��ѡ������߳�Ͷ��connect��һЩ��������������������ǲ�ȫ���ƻ��������unreg��������
		// add��unreg���л���ִ��
		// regͬʱ������̣߳�2������ǰδ��ȫ�˳�������Ҳ����add��
		// add��reg�����ڱ��߳��ٴα����ã�֮���reg��unreg������ͬһ�̱߳�����
		// TODO: �������Ƿ�ҲҪ
		if (isSocketRegistered_.load(std::memory_order_acquire))
		{
			return;
		}
		isSocketRegistered_.store(true, std::memory_order_release);
		// ���������õ���pri
		FDTaskCtlAttorney::Add(loop_, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP, this);
	}
}  // namespace Detail



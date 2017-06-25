#ifndef TLSPACKAGE_H_
#define TLSPACKAGE_H_
#include <memory>
#include <list>
#include "BlockPool.h"
#include "ConnectionPool.h"
#include "TimerManager.h"
namespace Detail
{
	class IConnection;
	class Loop;
	class Worker;
	// TODO: ����ṹ�����loop��Ҳ�ɣ���������loop�����̫�ණ��������ν��
	struct GlobalTlsPackage
	{
	public:
		friend Worker;
		~GlobalTlsPackage() = default;
		static GlobalTlsPackage& Instance();
		using IConnectionPtr = std::shared_ptr<IConnection>;
		using ConnList = std::list<IConnectionPtr>;
		BlockPool TlsBlockPool;
		ConnectionPool TlsConnPool;
		// �ݴ�ȴ��¼������ӣ���ֹͣloopʱ����������������
		ConnList TlsConnectionList;
		TimerManager TlsTimerManager;
		Loop* const TlsLoop;
	private:
		void SetLoop(Loop* loop);
		GlobalTlsPackage();

	};

}  // namespace Detail
#endif // TLSPACKAGE_H_


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
	// TODO: 这个结构如果放loop里也可，但是这样loop会耦合太多东西，看如何解决
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
		// 暂存等待事件的连接，在停止loop时会析构掉所有连接
		ConnList TlsConnectionList;
		TimerManager TlsTimerManager;
		Loop* const TlsLoop;
	private:
		void SetLoop(Loop* loop);
		GlobalTlsPackage();

	};

}  // namespace Detail
#endif // TLSPACKAGE_H_


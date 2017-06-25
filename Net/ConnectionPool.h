#ifndef CONNECTIONPOOL_H_
#define CONNECTIONPOOL_H_
#include <stack>
#include <memory>
#include "Noncopyable.h"
#include "TcpProactorConnection.h"
namespace Detail
{
	class TcpProactorConnection;
	class Loop;
	class ConnectionPool:
		public Noncopyable
	{
	public:
		using TcpConnUniPtr = std::unique_ptr<TcpProactorConnection>;
		using TcpConnPtr = std::shared_ptr<TcpProactorConnection>;
		ConnectionPool() = default;
		~ConnectionPool() = default;
		TcpConnPtr GetTcpConn(Loop& loop, int fd);
	private:
		std::stack<TcpConnUniPtr> tcp_;
		// 最大存储数量
		static constexpr int MaxSize = 1024;
	};

}  // namespace Detail

#endif // CONNECTIONPOOL_H_


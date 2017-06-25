#ifndef CONNECTIONATTORNEY_H_
#define CONNECTIONATTORNEY_H_

namespace Detail
{
	class ProactorConnection;
	class TcpProactorConnection;
	class Loop;
	class ConnectionAttorney
	{
	public:
		static void Recycle(ProactorConnection& conn);

		static void Reset(TcpProactorConnection& conn, Loop& loop,int fd);
	private:
		ConnectionAttorney() = delete;
		~ConnectionAttorney() = delete;

	};
}  // namespace Detail
#endif // CONNECTIONATTORNEY_H_


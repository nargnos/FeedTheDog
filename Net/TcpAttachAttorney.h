#ifndef TCPATTACHATTORNEY_H_
#define TCPATTACHATTORNEY_H_
#include <memory>
namespace Detail
{
	class TcpProactorConnection;
	class Loop;
	class TcpAttachAttorney
	{
	public:
		static std::shared_ptr<TcpProactorConnection> Attach(Loop& loop, int fd);
	private:
		TcpAttachAttorney() = delete;

		~TcpAttachAttorney() = delete;
	};


}  // namespace Detail
#endif // TCPATTACHATTORNEY_H_


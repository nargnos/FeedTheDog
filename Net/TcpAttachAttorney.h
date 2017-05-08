#pragma once
#include <memory>
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


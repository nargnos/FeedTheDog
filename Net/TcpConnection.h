
#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_
#include <memory>
#include <queue>
#include "IFDTask.h"
#include "ObjStore.h"
#include "SocketCpp.h"
#include "Connection.h"
#include "TransRec.h"
class TcpConnection :
	public Connection<TcpConnection>,
	public std::enable_shared_from_this<TcpConnection>
{
public:
	friend Connection<TcpConnection>;
	using Rec = TransRec<TcpConnection>;
	using RecPtr = std::unique_ptr<Rec>;
	using CompleteHandler = typename Rec::CompleteHandler;
	virtual ~TcpConnection();

	virtual int FD() const;
	// FIX: 这里不提供给用户
	static std::shared_ptr<TcpConnection> Attach(Loop& loop, int fd);
	// TODO: static std::shared_ptr<TcpConnection> Create(Loop& loop);

	void AsyncRead(Buffer&& buffer, size_t begin, CompleteHandler&& h);
	void AsyncReadSome(Buffer&& buffer, size_t begin, CompleteHandler&& h);
	void AsyncWrite(Buffer&& buffer, size_t begin, CompleteHandler&& h);

	// TODO: void AsyncConnect 地址直接用还是封装一个？

protected:
	TcpConnection(Loop& loop, int fd);
	explicit TcpConnection(Loop& loop);
	TcpSocket& Socket();
	const TcpSocket& Socket() const;
	void DispatchReadIoStatus(Status s);
	void DispatchWriteIoStatus(Status s);
	void PostCompleteHandler(std::queue<RecPtr>& queue, Error error);
	void OnReadSuccess();
	void OnWriteSuccess();
	void InitTask();
	bool IsReadyToRead()const ;
	bool IsReadyToWrite()const ;
	void OnReadFaild(Error e);
	void OnWriteFaild(Error e) ;
	void PostReadCompleteHandler(Error error);
	void PostWriteCompleteHandler(Error error);
	Status DoRead() ;
	Status DoWrite() ;
private:
	std::queue<RecPtr> readCompleteHandlers_;
	std::queue<RecPtr> writeCompleteHandlers_;
	TcpSocket socket_;
};


#endif // !TCPCONNECTION_H_

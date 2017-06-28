#ifndef TCPOPERATOR_H_
#define TCPOPERATOR_H_
#include <utility>
#include <memory>
#include <errno.h>
#include "Operator.h"
#include "Buffer.h"
#include "Progress.h"
#include "SocketCpp.h"
#include "GetSocketAttorney.h"
#include "Util.h"
namespace Detail
{
	void AddProgress(Progress& p, size_t trans, Error& e);

	// FIX: ȥ���࣬�Ż��㷨����ȡ���ԣ���չudp
	template<typename TConnection, typename TCallback>
	class TcpReadSomeOperator :
		public OperatorImpl<TcpReadSomeOperator<TConnection, TCallback>>
	{
	public:
		TcpReadSomeOperator(TConnection& conn, TCallback&& cb,size_t limit) :
			buff_(limit),
			cb_(std::forward<TCallback>(cb)),
			conn_(conn)
		{
		}

		void OnComplete(Error e)
		{
			cb_(conn_, std::move(buff_), e);
		}

		void OnOperator(Error& error)
		{
			auto& s = GetSocketAttorney::Socket(conn_);
			auto& block = buff_.Blocks().front();
			assert(block);
			auto reads = s.Read(block->data(), block->size());
			switch (reads)
			{
			case -1:
				SetErrno(error);
				break;
			default:
				block->resize(reads);
				SetErrStat(error, ErrStat::Success);
				break;
			case 0:
				SetErrStat(error, ErrStat::Close);
				break;
			}
		}
	private:
		Buffer buff_;
		TCallback cb_;
		TConnection& conn_;
	};
	template<typename TConnection, typename TCallback>
	std::unique_ptr<TcpReadSomeOperator<TConnection, TCallback>> MakeReadSomeOp(TConnection& conn, TCallback&& cb, size_t limit)
	{
		return std::make_unique<TcpReadSomeOperator<TConnection, TCallback>>(conn, std::forward<TCallback>(cb), limit);
	}

	
	template<typename TConnection, typename TCallback>
	class TcpWriteOperator :
		public OperatorImpl<TcpWriteOperator<TConnection, TCallback>>
	{
	public:
		TcpWriteOperator(TConnection& conn, Buffer&& buff, size_t begin, TCallback&& cb) :
			buff_(std::move(buff)),
			progress_(buff_.GetIovec()),
			cb_(std::forward<TCallback>(cb)),
			conn_(conn)
		{
			if (begin > 0)
			{
				progress_.Step(begin);
			}
			assert(progress_.Count() > 0);

		}
		void OnComplete(Error e)
		{
			// д����Ҫԭ�ȵ�������
			cb_(conn_, std::move(buff_), e);
		}
		void OnOperator(Error& error)
		{
			TcpSocket& s = GetSocketAttorney::Socket(conn_);
			auto ptr = progress_.Iovec();
			auto count = progress_.Count();
			assert(count > 0);
			auto trans = s.Writev(ptr, static_cast<int>(count));
			if (LIKELY(trans > 0))
			{
				AddProgress(progress_, trans, error);
			}
			else
			{
				SetErrno(error);
			}
		}
	private:
		Buffer buff_;
		Progress progress_;
		TCallback cb_;
		TConnection& conn_;
	};
	template<typename TConnection, typename TCallback>
	std::unique_ptr<TcpWriteOperator<TConnection, TCallback>> MakeWriteOp(TConnection& conn,
		Buffer&& buff, size_t begin, TCallback&& cb)
	{
		return std::make_unique<TcpWriteOperator<TConnection, TCallback>>(
			conn, std::move(buff), begin, std::forward<TCallback>(cb));
	}

	template<typename TConnection, typename TCallback>
	class TcpReadOperator :
		public OperatorImpl<TcpReadOperator<TConnection, TCallback>>
	{
	public:
		TcpReadOperator(TConnection& conn, size_t size, TCallback&& cb) :
			buff_(size),
			progress_(buff_.GetIovec()),
			cb_(std::forward<TCallback>(cb)),
			conn_(conn)
		{
			assert(progress_.Count() > 0);
		}
		void OnComplete(Error e)
		{
			cb_(conn_, std::move(buff_), e);
		}
		void OnOperator(Error& error)
		{
			TcpSocket& s = GetSocketAttorney::Socket(conn_);
			auto ptr = progress_.Iovec();
			auto count = progress_.Count();
			assert(count > 0);
			auto trans = s.Readv(ptr, count);
			switch (trans)
			{
			default:
				// ��ӽ���
				AddProgress(progress_, trans, error);
				break;
			case -1:
				SetErrno(error);
				break;
			case 0:
				SetErrStat(error, ErrStat::Close);
				break;
			}
		}
	private:
		Buffer buff_;
		Progress progress_;
		TCallback cb_;
		TConnection& conn_;
	};
	template<typename TConnection, typename TCallback>
	std::unique_ptr<TcpReadOperator<TConnection, TCallback>> MakeReadOp(TConnection& conn,
		size_t size, TCallback&& cb)
	{
		return std::make_unique<TcpReadOperator<TConnection, TCallback>>(
			conn, size, std::forward<TCallback>(cb));
	}



	// �ְ���,PrefixType Ϊ���ݳ���ǰ׺����
	template<typename TConnection, typename PrefixType, typename TCallback, typename TPrefixHandler>
	class TcpReadMessageOperator :
		public OperatorImpl<TcpReadMessageOperator<TConnection, PrefixType, TCallback, TPrefixHandler>>
	{
	public:
		static_assert(std::is_pod<PrefixType>::value, "!is_pod");

		// �ص����ص����ݳ��ȱ�ʾʣ�೤�ȣ��Ѷ�����ͨ��buffer�õ�
		// ����THeadHandler�ص�ȷ�����ݴ�С
		//  ssize_t (PrefixType) / ����Ϊ-1 ���� >=0 ��ʾʣ�����ݳ���
		// �������󷵻�false
		TcpReadMessageOperator(TConnection& conn, TCallback&& cb, TPrefixHandler&& ph) :
			buff_(sizeof(PrefixType)),
			progress_(buff_.GetIovec()),
			ph_(std::forward<TPrefixHandler>(ph)),
			cb_(std::forward<TCallback>(cb)),
			conn_(conn),
			hasHeader_(false)
		{
			assert(progress_.Count() > 0);
		}
		void OnComplete(Error e)
		{
			cb_(conn_, std::move(buff_), e);
		}

		void OnOperator(Error& error)
		{
			TcpSocket& s = GetSocketAttorney::Socket(conn_);
			auto ptr = progress_.Iovec();
			auto count = progress_.Count();
			assert(count > 0);
			auto trans = s.Readv(ptr, count);
			switch (trans)
			{
			default:
				// ��ӽ���
				if (progress_.Step(trans) == 0)
				{
					if (!hasHeader_)
					{
						// ����Ҫ���ô�С��
						auto pfx = *reinterpret_cast<PrefixType*>(buff_.Blocks().front()->data());
						assert(buff_.Size() == sizeof(PrefixType));
						ssize_t left = ph_(pfx);
						if (left < 0)
						{
							SetErrStat(error, ErrStat::Error);
							return;
						}
						assert(left >= 0);
						hasHeader_ = true;
						if (left > 0)
						{
							buff_.PushSize(left);
							// ����ʣ�����
							progress_ = Progress(buff_.GetIovec());
							auto ret = progress_.Step(sizeof(PrefixType));
							assert(ret > 0);
							SetErrStat(error, ErrStat::Continue);
							return;
						}
					}
					SetErrStat(error, ErrStat::Success);
				}
				else
				{
					SetErrStat(error, ErrStat::Continue);
				}
				break;
			case -1:
				SetErrno(error);
				break;
			case 0:
				SetErrStat(error, ErrStat::Close);
				break;
			}
		}
	private:
		Buffer buff_;
		Progress progress_;
		TPrefixHandler ph_;
		TCallback cb_;
		TConnection& conn_;
		bool hasHeader_;
	};
	template<typename PrefixType, typename TConnection, typename TCallback, typename TPrefixHandler>
	std::unique_ptr<TcpReadMessageOperator<TConnection, PrefixType, TCallback, TPrefixHandler>>
		MakeReadMessageOp(TConnection& conn, TCallback&& cb, TPrefixHandler&& ph)
	{
		return std::make_unique<TcpReadMessageOperator<TConnection, PrefixType, TCallback, TPrefixHandler>>(
			conn, std::forward<TCallback>(cb), std::forward<TPrefixHandler>(ph));
	}
}  // namespace Detail


#endif // TCPOPERATOR_H_


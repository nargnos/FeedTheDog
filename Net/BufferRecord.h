#ifndef BUFFERRECORD_H_
#define BUFFERRECORD_H_

#include <cassert>
#include <vector>
#include <tuple>
#include "Noncopyable.h"
#include "Buffer.h"

enum class Error :char
{
	Success,
	Close,
	Error
};

struct TransSome
{ };
class BufferRecord :
	public Noncopyable
{
public:
	using IOVecIterator = std::vector<iovec>::iterator;
	BufferRecord(Buffer&& buff, size_t begin) :
		buffer_(std::move(buff)),
		trans_(begin),
		isTransSome_(false)
	{
		assert(!buffer_.IsEmpty());
		auto& list = buffer_.VecList();

		snapshot_.reserve(list.size());
		max_ = 0;
		if (__glibc_likely(begin == 0))
		{
			for (const auto& i : list)
			{
				auto size = i->size();
				max_ += size;
				iovec v{ i->data(), size };
				snapshot_.push_back(v);
			}
		}
		else
		{
			for (const auto& i : list)
			{
				auto size = i->size();
				if (begin >= size)
				{
					begin -= size;
					continue;
				}

				char* data = i->data();

				if (begin != 0)
				{
					data += begin;
					size -= begin;
					begin = 0;
				}
				max_ += size;
				iovec v{ data, size };
				snapshot_.push_back(v);
			}
		}

		iovecBgn_ = snapshot_.begin();
		iovecEnd_ = snapshot_.end();
	}
	BufferRecord(Buffer&& buff, size_t begin, TransSome t) :
		BufferRecord(std::move(buff), begin)
	{
		isTransSome_ = true;
	}
	virtual ~BufferRecord() = default;
	bool IsComplete() const
	{
		return IsTransSome() || (max_ == Trans());
	}
	size_t Trans() const
	{
		return trans_;
	}
	size_t Left() const
	{
		assert(max_ >= trans_);
		return max_ - trans_;
	}
	void AddTrans(size_t newTrans)
	{
		if (newTrans == 0)
		{
			return;
		}
		assert(newTrans + trans_ <= max_);
		trans_ += newTrans;

		IOVecIterator& it = iovecBgn_;
		while (it != iovecEnd_)
		{
			auto& nodeLen = it->iov_len;
			if (nodeLen <= newTrans)
			{
				newTrans -= nodeLen;
				nodeLen = 0;
			}
			else
			{
				nodeLen -= newTrans;
				it->iov_base = (char*)it->iov_base + newTrans;
				break;
			}
			++it;
		}
	}
	const Buffer& GetBuffer() const
	{
		return buffer_;
	}
	Buffer& GetBuffer()
	{
		return buffer_;
	}

	IOVecIterator IOVecBgn() const
	{
		return iovecBgn_;
	}
	IOVecIterator IOVecEnd() const
	{
		return iovecEnd_;
	}

	bool IsTransSome() const
	{
		return isTransSome_;
	}
private:
	std::vector<iovec> snapshot_;
	Buffer buffer_;
	IOVecIterator iovecBgn_;
	IOVecIterator iovecEnd_;
	size_t trans_;
	size_t max_;
	bool isTransSome_;
};

template<typename TConn>
class TcpRecord :
	public BufferRecord
{
public:
	using THandler = std::function<void(TConn*, Buffer&&, Error)>;
	TcpRecord(Buffer&& buff, size_t begin, THandler&& func, TransSome t) :
		BufferRecord(std::move(buff), begin, t),
		cb_(std::move(func))
	{

	}

	TcpRecord(Buffer&& buff, size_t begin, THandler&& func) :
		BufferRecord(std::move(buff), begin),
		cb_(std::move(func))
	{

	}
	void Call(TConn* conn, Buffer&& buf, Error e) const
	{
		cb_(conn, std::move(buf), e);
	}
private:
	THandler cb_;
};




#endif // BUFFERRECORD_H_


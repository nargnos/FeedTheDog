#ifndef TRANSFERPROGRESS_H_
#define TRANSFERPROGRESS_H_
#include "Buffer.h"

struct TransSome
{ };

class TransferProgress :
	public Noncopyable
{
public:
	using IOVecIterator = std::vector<iovec>::iterator;
	TransferProgress(Buffer&& buff, size_t begin, bool isTransSome) :
		buffer_(std::move(buff)),
		trans_(begin),
		max_(0),
		isTransSome_(isTransSome)
	{
		assert(!buffer_.IsEmpty());
		auto& list = buffer_.VecList();

		snapshot_.reserve(list.size());
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
		assert(max_ > 0);
		iovecBgn_ = snapshot_.begin();
		iovecEnd_ = snapshot_.end();
	}

	virtual ~TransferProgress() = default;
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
#endif // TRANSFERPROGRESS_H_


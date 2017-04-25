#ifndef TRANSREC_H_
#define TRANSREC_H_
#include <memory>
#include <functional>
#include <numeric>
#include <tuple>
#include "Buffer.h"
#include "ITask.h"
#include "Noncopyable.h"
// 封装errno可能好些？
enum class Error :char
{
	Success,
	Close,
	Error
};
struct TransSome
{ };
// 这里用接口可以省掉function
// TODO: CompleteHandler 应该用模板，function好像不能被优化
template<typename TConnection>
class TransRec :
	public ITask,
	public Noncopyable
{
public:
	using ConnectionPtr = std::shared_ptr<TConnection>;
	using ConnectionPtrMv = ConnectionPtr&&;
	using CompleteHandler = std::function<void(TConnection*, Buffer&&, Error)>;
	using IOVecIterator = std::vector<iovec>::iterator;
	TransRec(ConnectionPtrMv conn, Buffer&& buff, CompleteHandler&& e, size_t begin) :
		cb_(std::move(e)),
		buffer_(std::move(buff)),
		conn_(std::move(conn)),
		trans_(begin),
		err_(Error::Success),
		isTransSome_(false)
	{
		assert(!buffer_.IsEmpty());
		auto& list = buffer_.VecList();
		snapshot_.reserve(list.size());
		max_ = 0;
		for (const auto& i : list)
		{
			auto size = i->size();
			if (begin >= size)
			{
				begin -= size;
				continue;
			}

			iovec v;
			const char* data = i->data();

			if (begin != 0)
			{
				data += begin;
				size -= begin;
				begin = 0;
			}
			v.iov_base = const_cast<char*>(data);
			v.iov_len = size;
			max_ += size;
			snapshot_.push_back(v);
		}

		iovec_ = std::make_tuple(snapshot_.begin(), snapshot_.end());
	}

	TransRec(ConnectionPtrMv conn, Buffer&& buff, CompleteHandler&& e, size_t begin, TransSome t) :
		TransRec(std::move(conn), std::move(buff), std::move(e), begin)
	{
		isTransSome_ = true;
	}
	~TransRec() = default;
	virtual bool DoEvent(Loop& loop, std::unique_ptr<ITask>&& self) override
	{
		cb_(&*conn_, std::move(buffer_), err_);
		return true;
	}
	void SetError(Error err)
	{
		err_ = err;
	}
	bool IsComplete() const
	{
		return Left() == 0;
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

		IOVecIterator& it = std::get<0>(iovec_);
		IOVecIterator end = std::get<1>(iovec_);
		while (it != end)
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
	const std::tuple<IOVecIterator, IOVecIterator>& IOVec()
	{
		return iovec_;
	}
	CompleteHandler& Callback()
	{
		return cb_;
	}
	bool IsTransSome() const
	{
		return isTransSome_;
	}
private:
	CompleteHandler cb_;
	std::tuple<IOVecIterator, IOVecIterator> iovec_;
	std::vector<iovec> snapshot_;
	Buffer buffer_;
	ConnectionPtr conn_;
	size_t trans_;
	size_t max_;
	Error err_;
	bool isTransSome_;
};


#endif // TRANSREC_H_


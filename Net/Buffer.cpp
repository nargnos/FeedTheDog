#include "Buffer.h"
#include <numeric>
#include "TlsPackage.h"
namespace Detail
{
	Buffer::Buffer(size_t size)
	{
		PushSize(size);
	}

	Buffer::Buffer(const BlockPtr & vec)
	{
		assert(vec);
		list_.push_back(vec);
	}

	Buffer::Buffer(Buffer && val) :
		list_(std::move(val.list_))
	{
		// list有自身判断
	}

	bool Buffer::IsEmpty() const
	{
		return list_.empty();
	}

	size_t Buffer::Size() const
	{
		return Size(*this);
	}

	BlockPtr Buffer::New()
	{
		return BufferPool().New();
	}

	BlockPtr Buffer::New(size_t size)
	{
		return BufferPool().New(size);
	}

	// 保留各自vec大小，只在最后元素修改

	void Buffer::Resize(size_t size)
	{
		// 等于0的buffer没有意义
		if (size == 0)
		{
			if (!list_.empty())
			{
				list_.erase(++list_.begin(), list_.end());
				list_.front()->resize(0);
			}
			return;
		}
		auto it = list_.begin();
		auto end = list_.end();

		while (it != end)
		{
			auto& v = *it;
			auto tmp = v->size();
			if (__glibc_unlikely(tmp == 0))
			{
				list_.erase(it++);
				continue;
			}
			if (__glibc_unlikely(size >= tmp))
			{
				size -= tmp;
				++it;
			}
			else if (size == 0)
			{
				list_.erase(it, end);
			}
			else
			{
				v->resize(size);
				size = 0;
				list_.erase(++it, end);
			}
		}

		if (size > 0)
		{
			PushSize(size);
		}
	}


	void Buffer::PushSize(size_t size)
	{
		if (size == 0)
		{
			list_.push_back(New());
			return;
		}
		while (size != 0)
		{
			auto ptr = New(size);
			assert(ptr->size() <= size);
			size -= ptr->size();
			list_.push_back(ptr);
		}
	}

	void Buffer::StretchTo(size_t size)
	{
		if (size == 0)
		{
			for (auto& i : list_)
			{
				i->ResizeToMax();
			}
			return;
		}

		auto it = list_.begin();
		auto end = list_.end();

		while (it != end)
		{
			auto& v = *it;
			auto max = v->max_size();
			assert(max > 0);
			if (size >= max)
			{
				v->ResizeToMax();
				size -= max;
				++it;
			}
			else if (size == 0)
			{
				list_.erase(it, end);
			}
			else
			{
				v->resize(size);
				size = 0;
				list_.erase(++it, end);
				break;
			}
		}
		if (size > 0)
		{
			PushSize(size);
		}
	}

	Buffer::BlockList& Buffer::Blocks()
	{
		return list_;
	}

	const Buffer::BlockList & Buffer::Blocks() const
	{
		return list_;
	}

	BlockPool& Buffer::BufferPool()
	{
		return GlobalTlsPackage::Instance().TlsBlockPool;
	}

	size_t Buffer::Size(const std::vector<iovec>& iov)
	{
		return std::accumulate(iov.begin(), iov.end(), 0,
			[](size_t val, const iovec& item) {
			return val + item.iov_len;
		});
	}

	size_t Buffer::Size(const Buffer & buf)
	{
		auto& blocks = buf.Blocks();
		return std::accumulate(blocks.begin(), blocks.end(), 0,
			[](size_t val, const BlockPtr& ptr) {
			return val + ptr->size();
		});
	}

	std::vector<iovec> Buffer::GetIovec() const
	{
		std::vector<iovec> result(0);
		result.reserve(Size());
		for (auto& item : Blocks())
		{
			result.push_back(item->GetIOV());
		}
		return result;
	}

}  // namespace Detail
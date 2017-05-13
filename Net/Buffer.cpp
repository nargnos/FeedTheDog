﻿#include "Buffer.h"
#include <numeric>
namespace Detail
{
	Buffer::Buffer(size_t size)
	{
		list_.push_front(Buffer::BufferPool().New(size));
	}

	Buffer::Buffer(const BlockPtr & vec)
	{
		assert(vec);
		PushBack(vec);
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

	const BlockPtr & Buffer::EmplaceBack(size_t size)
	{
		return *list_.insert(list_.end(), BufferPool().New(size));
	}

	const BlockPtr & Buffer::EmplaceFront(size_t size)
	{
		return *list_.insert(list_.begin(), BufferPool().New(size));
	}

	void Buffer::PushBack(const BlockPtr & buf)
	{
		assert(buf);
		list_.push_back(buf);
	}

	void Buffer::PushFront(const BlockPtr & buf)
	{
		assert(buf);
		list_.push_front(buf);
	}

	void Buffer::PushBack(BlockPtr && buf)
	{
		assert(buf);
		list_.push_back(std::move(buf));
	}

	void Buffer::PushFront(BlockPtr && buf)
	{
		assert(buf);
		list_.push_front(std::move(buf));
	}

	void Buffer::PopBack()
	{
		assert(!IsEmpty());
		list_.pop_back();
	}

	void Buffer::PopFront()
	{
		assert(!IsEmpty());
		list_.pop_front();
	}


	size_t Buffer::Size() const
	{
		return Size(*this);
	}

	void Buffer::Resize(size_t size)
	{
		// 等于0的buffer没有意义
		assert(size > 0);
		if (size == 0)
		{
			return;
		}
		auto it = list_.begin();
		auto end = list_.end();

		bool needAppend = true;
		while (it != end)
		{
			auto& v = *it;
			auto tmp = v->size();
			if (__glibc_unlikely(size > tmp))
			{
				size -= tmp;
			}
			else
			{
				v->resize(size);
				list_.erase(++it, end);
				needAppend = false;
				break;
			}
			++it;
		}

		if (needAppend)
		{
			EmplaceBackTotal(size);
		}
	}


	// 伸缩大小，扩展到size大小，会修改每个元素的size, 0表示把每个元素都扩展到max

	 void Buffer::StretchTo(size_t size)
	{
		assert(!IsEmpty());
		if (__glibc_likely(size == 0))
		{
			for (auto& i : list_)
			{
				i->ResizeToMax();
			}
			return;
		}
		auto it = list_.begin();
		auto end = list_.end();
		bool needAppend = true;
		while (it != end)
		{
			auto& v = *it;
			assert(!v->IsReadOnly());
			auto cap = v->max_size();
			if (size > cap)
			{
				size -= cap;
				v->resize(cap);
			}
			else
			{
				v->resize(size);
				list_.erase(++it, end);
				needAppend = false;
				break;
			}
			++it;
		}

		if (needAppend)
		{
			EmplaceBackTotal(size);
		}
	}

	const Buffer::BlockList & Buffer::VecList() const
	{
		return list_;
	}

	BlockPool& Buffer::BufferPool()
	{
		static thread_local BlockPool result;
		return result;
	}

	size_t Buffer::Size(const std::vector<iovec>& iov)
	{
		size_t result = 0;
		for (auto& i : iov)
		{
			result += i.iov_len;
		}
		return result;
	}

	size_t Buffer::Size(const Buffer & buf)
	{
		size_t result = 0;
		for (auto& i : buf.list_)
		{
			result += i->size();
		}
		return result;
	}

	bool Buffer::HasReadOnlyBlock() const
	{
		for (auto& i : list_)
		{
			if (i->IsReadOnly())
			{
				return true;
			}
		}
		return false;
	}

	void Buffer::DropReadOnlyBlock()
	{
		auto it = list_.begin();
		auto end = list_.end();
		for (; it != end; ++it)
		{
			if ((*it)->IsReadOnly())
			{
				it = list_.erase(it);
			}
		}
	}


	void Buffer::EmplaceBackTotal(size_t size)
	{
		int count = 0;
		while (true)
		{
			auto tmp = EmplaceBack(size)->size();
			assert(size >= tmp);
			if (size > tmp)
			{
				size -= tmp;
				++count;
				// 限制5倍MAX大小，申请的这个空间太大了
				assert(count <= 5);
			}
			else
			{
				break;
			}
		}
	}

}  // namespace Detail
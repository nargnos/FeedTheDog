#ifndef BUFFER_H_
#define BUFFER_H_
#include <memory>
#include <list>
#include <sys/uio.h>
#include "BlockPool.h"
#include "Noncopyable.h"
namespace Detail
{
	// 非线程安全
	class Buffer :public Noncopyable
	{
	public:
		// 使用shared_ptr存储，这样可以跟其它buffer共用一部分（发送）内容（固定文本什么的），但注意某些函数会修改到元素的大小	
		using BlockList = std::list<BlockPtr>;
		explicit Buffer(size_t size);
		explicit Buffer(const BlockPtr& vec);
		// 注意此时未分配空间
		Buffer() = default;

		Buffer(Buffer&& val);

		~Buffer() = default;
		bool IsEmpty() const;

		size_t Size()const;
		static BlockPtr New();
		static BlockPtr New(size_t size);
		// 保留各自vec大小，只在最后元素修改
		void Resize(size_t size);
		// 填充指定大小block, 0为填充默认大小
		void PushSize(size_t size);
		// 0表示设置为最大
		void StretchTo(size_t size);
		BlockList& Blocks();
		const BlockList& Blocks() const;
		static BlockPool& BufferPool();
		static size_t Size(const std::vector<iovec>& iov);
		static size_t Size(const Buffer& buf);
		std::vector<iovec> GetIovec() const;
		// 浅拷贝
		Buffer Clone() const;
	private:
		explicit Buffer(const BlockList& list);
		BlockList list_;
	};
	
}  // namespace Detail
using Detail::Buffer;
#endif // BUFFER_H_


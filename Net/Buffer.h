#ifndef BUFFER_H_
#define BUFFER_H_
#include <memory>
#include <list>
#include <sys/uio.h>
#include "BlockPool.h"
#include "Noncopyable.h"
// 非线程安全，不能给其它线程用
class Buffer :public Noncopyable
{
public:
	// 使用shared_ptr存储，这样可以跟其它buffer共用一部分（发送）内容（固定文本什么的），但注意某些函数会修改到元素的大小	
	using BlockList = std::list<BlockPtr>;
	explicit Buffer(size_t size);
	explicit Buffer(const BlockPtr& vec);
	Buffer() = default;

	Buffer(Buffer&& val);

	~Buffer() = default;
	bool IsEmpty() const;
	
	const BlockPtr& EmplaceBack(size_t size);
	const BlockPtr& EmplaceFront(size_t size);
	void PushBack(const BlockPtr& buf);
	void PushFront(const BlockPtr& buf);
	void PopBack();
	void PopFront();
	size_t Size()const;
	// 保留各自vec大小，只在最后元素修改
	void Resize(size_t size);
	// 伸缩大小，扩展到size大小，会修改每个元素的size, 0表示把每个元素都扩展到max
	void StretchTo(size_t size = 0);
	const BlockList& VecList() const;
	static BlockPool& BufferPool();
	static size_t Size(const std::vector<iovec>& iov);
	static size_t Size(const Buffer& buf);
private:
	// 总填充size
	void EmplaceBackTotal(size_t size);
	BlockList list_;
};

#endif // BUFFER_H_


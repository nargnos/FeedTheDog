#ifndef BUFFER_H_
#define BUFFER_H_
#include <memory>
#include <list>
#include <sys/uio.h>
#include "BlockPool.h"
#include "Noncopyable.h"
// ���̰߳�ȫ�����ܸ������߳���
class Buffer :public Noncopyable
{
public:
	// ʹ��shared_ptr�洢���������Ը�����buffer����һ���֣����ͣ����ݣ��̶��ı�ʲô�ģ�����ע��ĳЩ�������޸ĵ�Ԫ�صĴ�С	
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
	// ��������vec��С��ֻ�����Ԫ���޸�
	void Resize(size_t size);
	// ������С����չ��size��С�����޸�ÿ��Ԫ�ص�size, 0��ʾ��ÿ��Ԫ�ض���չ��max
	void StretchTo(size_t size = 0);
	const BlockList& VecList() const;
	static BlockPool& BufferPool();
	static size_t Size(const std::vector<iovec>& iov);
	static size_t Size(const Buffer& buf);
private:
	// �����size
	void EmplaceBackTotal(size_t size);
	BlockList list_;
};

#endif // BUFFER_H_


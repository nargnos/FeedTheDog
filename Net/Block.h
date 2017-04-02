#ifndef BLOCK_H_
#define BLOCK_H_
#include <iterator>
#include <cassert>
#include <memory>
#include <vector>
#include <string>
#include "Noncopyable.h"
// 配合stl的一些算法，函数名用默认风格
class Block :public Noncopyable
{
public:
	static constexpr size_t Align = 64;
	typedef char value_type;
	typedef char* pointer;
	typedef const char*  const_pointer;
	typedef char& reference;
	typedef const char& const_reference;
	typedef char* iterator;
	typedef const char* const_iterator;
	typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef size_t size_type;
	typedef std::ptrdiff_t difference_type;

	Block(void* ptr, size_t max, size_t size);
	virtual ~Block();
	iterator begin();
	iterator end();
	iterator data();
	const_iterator data()const;
	const_iterator begin() const;
	const_iterator end() const;
	reference operator[](int index);
	const_reference operator[](int index) const;
	size_t size() const;
	size_t max_size() const;
	void resize(size_t newsize);
	void ResizeToMax();
protected:
	static void* Malloc(size_t size);
	static void Free(void* ptr);
	char* ptr_;
	size_t max_;
	size_t size_;
};

class SmallBlock :public Block
{
public:
	constexpr static size_t Max = 128;
	SmallBlock(size_t size);
	SmallBlock();
	virtual ~SmallBlock();
};

class BigBlock :public Block
{
public:
	constexpr static size_t Max = 1 << 18;
	BigBlock(size_t size);
	BigBlock();
	virtual ~BigBlock();;
};

// 做一个封装，方便组合
class VectorBlock :public Block
{
public:
	VectorBlock(const std::shared_ptr<std::vector<char>>& vec);
private:
	std::shared_ptr<std::vector<char>> vec_;
};
// 不安全，封装用来发送的，要注意不要被修改到
// FIX: 当尝试写时提示，用const或bool标识都不可以，转iovec的时候都丢失，但是发送时必须转
class StringBlock :public Block
{
public:
	StringBlock(const std::shared_ptr<std::string>& str);
private:
	std::shared_ptr<std::string> str_;
};

class CharBlock :public Block
{
public:
	CharBlock(const char* str, size_t len);
};

#endif // BLOCK_H_


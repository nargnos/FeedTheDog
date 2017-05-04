#include "Block.h"

BigBlock::BigBlock(size_t size) :
	Block(&buf_, Max, size, false)
{
	assert(size <= Max);
}

BigBlock::BigBlock() :BigBlock(Max)
{

}

BigBlock::~BigBlock() = default;

SmallBlock::SmallBlock(size_t size) :
	Block(&buf_, Max, size, false)
{
	assert(size <= Max);
}
SmallBlock::SmallBlock() : SmallBlock(Max)
{

}
SmallBlock::~SmallBlock() = default;
Block::reference Block::operator[](int index)
{
	assert(index > 0 && index < size_);
	return begin()[index];
}
Block::const_reference Block::operator[](int index) const
{
	assert(index > 0 && index < size_);
	return begin()[index];
}
Block::Block(void * ptr, size_t max, size_t size, bool isReadOnly) :
	ptr_(static_cast<char*>(ptr)),
	max_(max),
	size_(size),
	isReadOnly_(isReadOnly)
{
	assert(ptr_);
}

Block::~Block() = default;

Block::iterator Block::begin()
{
	assert(ptr_);
	return ptr_;
}

Block::iterator Block::end()
{
	return begin() + size_;
}

Block::iterator Block::data()
{
	return ptr_;
}

Block::const_iterator Block::data() const
{
	return ptr_;
}

Block::const_iterator Block::begin() const
{
	assert(ptr_);
	return ptr_;
}

Block::const_iterator Block::end() const
{
	return begin() + size_;
}

size_t Block::size() const
{
	return size_;
}

size_t Block::max_size() const
{
	return max_;
}

void Block::resize(size_t newsize)
{
	assert(newsize <= max_);
	size_ = newsize;
}

void Block::ResizeToMax()
{
	size_ = max_;
}

bool Block::IsReadOnly() const
{
	return isReadOnly_;
}


VectorBlock::VectorBlock(const std::shared_ptr<std::vector<char>>& vec) :
	Block(vec->data(), vec->size(), vec->size(), true),
	vec_(vec)
{
}

StringBlock::StringBlock(const std::shared_ptr<std::string>& str) :
	Block(const_cast<char*>(str->c_str()), str->length(), str->length(), true),
	str_(str)
{
}

CharBlock::CharBlock(const char * str, size_t len) :
	Block(const_cast<char*>(str), len, len, true)
{
}

#include "Block.h"
namespace Detail
{

	Block::reference Block::operator[](int index)
	{
		assert(index > 0 && index < size());
		return begin()[index];
	}
	Block::const_reference Block::operator[](int index) const
	{
		assert(index > 0 && index < size());
		return begin()[index];
	}
	Block::Block(void * ptr, size_t max, size_t size, bool isReadOnly) :
		iov_{ ptr,size },
		max_(max),
		isReadOnly_(isReadOnly)
	{
		assert(ptr);
	}


	Block::~Block() = default;

	Block::iterator Block::begin()
	{
		assert(iov_.iov_base);
		return static_cast<char*>(iov_.iov_base);
	}

	Block::iterator Block::end()
	{
		return begin() + size();
	}

	Block::iterator Block::data()
	{
		return begin();
	}

	Block::const_iterator Block::data() const
	{
		return begin();
	}

	Block::const_iterator Block::begin() const
	{
		assert(iov_.iov_base);
		return static_cast<char*>(iov_.iov_base);
	}

	Block::const_iterator Block::end() const
	{
		return begin() + size();
	}

	size_t Block::size() const
	{
		return iov_.iov_len;
	}

	size_t Block::max_size() const
	{
		return max_;
	}

	void Block::resize(size_t newsize)
	{
		assert(newsize <= max_);
		iov_.iov_len = newsize;
	}

	bool Block::IsReadOnly() const
	{
		return isReadOnly_;
	}

	const iovec& Block::GetIOV() const
	{
		return iov_;
	}


	BigBlock::BigBlock(size_t size) :
		Block(&buf_, Max, size, false)
	{
		assert(size <= Max);
	}


	BigBlock::~BigBlock() = default;

	SmallBlock::SmallBlock(size_t size) :
		Block(&buf_, Max, size, false)
	{
		assert(size <= Max);
	}
	SmallBlock::~SmallBlock() = default;

	VectorBlock::VectorBlock(const std::vector<char>& vec) :
		VectorBlock::Member(vec),
		Block(VectorBlock::Member::Member().data(),
			VectorBlock::Member::Member().size(),
			VectorBlock::Member::Member().size(),
			true)
	{
	}

	VectorBlock::VectorBlock(std::vector<char>&& vec) :
		VectorBlock::Member(std::move(vec)),
		Block(VectorBlock::Member::Member().data(),
			VectorBlock::Member::Member().size(),
			VectorBlock::Member::Member().size(),
			true)
	{

	}

	StringBlock::StringBlock(const std::string& str) :
		StringBlock::Member(str),
		Block(const_cast<char*>(StringBlock::Member::Member().data()),
			StringBlock::Member::Member().length(),
			StringBlock::Member::Member().length(),
			true)
	{
	}
	StringBlock::StringBlock(std::string && str) :
		StringBlock::Member(std::move(str)),
		Block(const_cast<char*>(StringBlock::Member::Member().data()),
			StringBlock::Member::Member().length(),
			StringBlock::Member::Member().length(),
			true)
	{
		
	}

	CharBlock::CharBlock(const char * str, size_t len) :
		Block(const_cast<char*>(str), len, len, true)
	{
	}


}  // namespace Detail
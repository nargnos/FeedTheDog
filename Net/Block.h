#ifndef BLOCK_H_
#define BLOCK_H_
#include <iterator>
#include <cassert>
#include <memory>
#include <vector>
#include <string>
#include <sys/socket.h>
#include "Noncopyable.h"
#include "BaseFromMember.h"
namespace Detail
{
	// 配合stl的一些算法，函数名用默认风格
	// TODO: 想改成支持切片的那种
	class Block :
		public Noncopyable
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
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef size_t size_type;
		typedef std::ptrdiff_t difference_type;

		Block(void* ptr, size_t max, size_t size, bool isReadOnly);
		virtual ~Block();
		bool IsReadOnly() const;
		const_iterator begin() const;
		const_iterator data()const;
		const_iterator end() const;
		const_reference operator[](int index) const;
		const iovec& GetIOV() const;
		iterator begin();
		iterator data();
		iterator end();
		reference operator[](int index);
		size_t max_size() const;
		size_t size() const;
		void resize(size_t newsize);
		inline void ResizeToMax()
		{
			iov_.iov_len = max_;
		}
	protected:
		// buffer指针和当前设置大小存里面
		iovec iov_;
		const size_t max_;
		const bool isReadOnly_;
	};

	class SmallBlock :
		public Block
	{
	public:
		constexpr static size_t Max = 128;
		explicit SmallBlock(size_t size = Max);
		virtual ~SmallBlock();
	private:
		std::aligned_storage_t<Max, Align> buf_;
	};

	class BigBlock :
		public Block
	{
	public:
		constexpr static size_t Max = 1 << 18;
		explicit BigBlock(size_t size = Max);
		virtual ~BigBlock();
	private:
		std::aligned_storage_t<Max, Align> buf_;
	};

	// 做一个封装，方便组合
	class VectorBlock :
		protected BaseFromMember<std::vector<char>>,
		public Block
	{
	public:
		VectorBlock(const std::vector<char>& vec);
		VectorBlock(std::vector<char>&& vec);
	private:
		using Member = BaseFromMember<std::vector<char>>;
	};
	class StringBlock :
		protected BaseFromMember<std::string>,
		public Block
	{
	public:
		StringBlock(const std::string& str);
		StringBlock(std::string&& str);
	private:
		using Member = BaseFromMember<std::string>;

	};
	// 这里指的是全局str
	class CharBlock :
		public Block
	{
	public:
		CharBlock(const char* str, size_t len);
	};

}  // namespace Detail
using Detail::CharBlock;
using Detail::StringBlock;
using Detail::VectorBlock;

#endif // BLOCK_H_


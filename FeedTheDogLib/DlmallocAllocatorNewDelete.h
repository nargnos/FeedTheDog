#pragma once
namespace FeedTheDog
{
	// 其实修改成这个分配器对性能的提升微乎极微
	struct DlmallocAllocatorNewDelete
	{
		typedef _STD size_t size_type;
		typedef _STD ptrdiff_t difference_type;

		static char * malloc(const size_type bytes)
		{
			return reinterpret_cast<char*>(_BOOST container::dlmalloc_malloc(bytes));
		}
		static void free(char * const block)
		{
			_BOOST container::dlmalloc_free(block);
		}
	};
}  // namespace FeedTheDog

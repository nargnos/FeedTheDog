#pragma once
#include "Common.h"
namespace FeedTheDog
{
	class Config
	{
	public:
		static const int BufferSize = 1024 * 10;
		Config();
		~Config();
		void Load()
		{
			threadCount = _BOOST thread::hardware_concurrency() * 2;
		}
		int GetThreadCount()
		{
			return threadCount;
		}
	private:
		int threadCount;
	};
}  // namespace FeedTheDog

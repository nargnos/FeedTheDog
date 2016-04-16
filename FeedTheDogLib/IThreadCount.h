#pragma once
namespace FeedTheDog
{
	__interface IThreadCount
	{
		unsigned int GetThreadCount();
		unsigned int GetMaxThreadCount() const;
	};
}  // namespace FeedTheDog

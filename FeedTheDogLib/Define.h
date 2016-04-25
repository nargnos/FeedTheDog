#pragma once
namespace FeedTheDog
{
	template<typename>
	struct IService;
	template<typename>
	class Core;
	template<typename>
	class Worker;

	template<typename, typename, typename, typename, typename>
	class SessionPool;

	template<typename, typename>
	class Session;
	template<typename, typename>
	class SessionNoBuffer;

	template<typename>
	class WorkerPool;
}  // namespace FeedTheDog

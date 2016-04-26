#pragma once
namespace FeedTheDog
{
	template<typename>
	struct IService;
	template<typename>
	class Core;
	class Worker;

	template< typename, typename, typename, typename>
	class SessionPool;

	template<typename, typename,bool>
	class Session;
	class Config;
	class WorkerPool;
}  // namespace FeedTheDog

#pragma once
namespace FeedTheDog
{
	template<typename>
	struct IService;
	template<typename>
	class Core;
	class Worker;

	template<typename>
	class SessionPool;

	template<typename,bool>
	class Session;
	class Config;
	class WorkerPool;
}  // namespace FeedTheDog

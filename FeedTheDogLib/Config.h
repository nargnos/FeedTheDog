#pragma once
#include "TraceSource.h"
namespace FeedTheDog
{
	enum class LogMsg
	{
		Initialized,
		NewCore,
		FreeCore,
		NewWorker,
		FreeWorker,
		NewSessionPool,
		FreeSessionPool,
		NewSession,
		FreeSession,
		AllocMemory,
		FreeMemory,
		CoreStart,
		CoreStop,
		StartWorker,
		StopWorker,
		CloseAllSocket,
		MainEnd,
		Exit,
		AddService,
		DeleteService

	};
	class Config
	{
	public:
		static const int BufferSize = 1024 * 10;
		typedef LogMsg TEnum;
		Config();
		~Config();
		void Load();
		void Save();
		int GetThreadCount();
		int GetMaxThreadCount() const;
		shared_ptr<TraceSource<TEnum>>& GetTrace();
	private:
		int maxThreadCount;
		char* configPath;
		shared_ptr<TraceSource<TEnum>> trace;
		shared_ptr<typename TraceSource<TEnum>::TMap> texts;
		shared_ptr<Json::StreamWriter> writer;
		Json::Value root;
		Json::Value& ConfigNode();
		Json::Value& LoggerNode();

		Json::Value& ThreadCountMember();
	};
}  // namespace FeedTheDog

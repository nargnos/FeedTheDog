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
	// TODO: 功能待完善，文件那里要能按日期分
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

		Json::Value& ThreadCountMember();
	};
}  // namespace FeedTheDog

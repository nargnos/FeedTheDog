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
		
		// 1w多次正常浏览网页所需buffer大小平均为5000+（可能是浏览器限制）
		
		typedef LogMsg TEnum;
		typedef TraceSource<TEnum> TTraceSource;
		Config();
		~Config();
		void Load();
		void Save();
		unsigned int GetThreadCount();
		unsigned int GetMaxThreadCount() const;
		shared_ptr<TTraceSource>& GetTrace();
	private:
		unsigned int maxThreadCount;
		char* configPath;
		shared_ptr<TTraceSource> trace;
		shared_ptr<typename TTraceSource::TMap> texts;
		shared_ptr<Json::StreamWriter> writer;
		Json::Value root;
		Json::Value& ConfigNode();

		Json::Value& ThreadCountMember();
	};
}  // namespace FeedTheDog

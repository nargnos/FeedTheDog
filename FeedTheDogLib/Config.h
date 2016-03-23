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
	// TODO: ���ܴ����ƣ��ļ�����Ҫ�ܰ����ڷ�
	class Config
	{
	public:
		
		// 1w������������ҳ����buffer��Сƽ��Ϊ5000+����������������ƣ�
		static const unsigned int BufferSize = 1024 * 6;
		typedef LogMsg TEnum;
		typedef TraceSource<TEnum> TTraceSource;
		Config();
		~Config();
		void Load();
		void Save();
		int GetThreadCount();
		int GetMaxThreadCount() const;
		shared_ptr<TTraceSource>& GetTrace();
	private:
		int maxThreadCount;
		char* configPath;
		shared_ptr<TTraceSource> trace;
		shared_ptr<typename TTraceSource::TMap> texts;
		shared_ptr<Json::StreamWriter> writer;
		Json::Value root;
		Json::Value& ConfigNode();

		Json::Value& ThreadCountMember();
	};
}  // namespace FeedTheDog

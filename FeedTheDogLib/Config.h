#pragma once
#include "ITraceConfig.h"
#include "IThreadCount.h"
#include "TraceSource.h"
namespace FeedTheDog
{
	// TODO: 功能待完善，文件那里要能按日期分
	class Config:
		public TraceSource::TTraceConfig,
		public IThreadCount
	{
	public:
		Config();
		~Config();
		void Save();
		virtual unsigned int GetThreadCount() override;
		virtual unsigned int GetMaxThreadCount() const override;

		virtual bool IsOpenTrace() const override;
		virtual TListenerVector CreateListeners() override;
	private:
		unsigned int maxThreadCount;
		char* configPath;

		shared_ptr<Json::StreamWriter> writer;
		Json::Value root;
		Json::Value& ConfigNode();

		Json::Value & TraceNode();
		Json::Value & OpenTraceNode();
		Json::Value& ThreadCountMember();

		bool isOpenTrace;
		bool isEmpty;
	};
}  // namespace FeedTheDog

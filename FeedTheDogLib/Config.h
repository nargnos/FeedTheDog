#pragma once
#include "TraceSource.h"
namespace FeedTheDog
{
	// TODO: 功能待完善，文件那里要能按日期分
	class Config:public TraceSource::TTraceConfig
	{
	public:
		Config();
		~Config();
		void Save();
		unsigned int GetThreadCount();
		unsigned int GetMaxThreadCount() const;

	private:
		unsigned int maxThreadCount;
		char* configPath;

		shared_ptr<Json::StreamWriter> writer;
		Json::Value root;
		Json::Value& ConfigNode();

		Json::Value & TraceNode();
		Json::Value & IsOpenTraceNode();
		Json::Value& ThreadCountMember();

		virtual bool IsOpenTrace() const override;
		virtual TListenerVector CreateListeners() override;
		bool isOpenTrace;
		bool isEmpty;
	};
}  // namespace FeedTheDog

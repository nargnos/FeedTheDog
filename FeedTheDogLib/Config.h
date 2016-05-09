#pragma once
#include "ITraceConfig.h"
#include "TraceSource.h"
namespace FeedTheDog
{
	// TODO: 功能待完善，文件那里要能按日期分
	class Config :
		public TraceSource::TTraceConfig,
		public _BOOST noncopyable
	{
	public:
		Config();
		~Config();
		void Save();
		virtual bool IsOpenTrace() const override;
		virtual TListenerVector CreateListeners() override;
	private:
		Json::Value& ConfigNode();

		Json::Value & TraceNode();
		Json::Value & OpenTraceNode();

		Json::Value root; // 24 B

		shared_ptr<Json::StreamWriter> writer;
		char* configPath;
		bool isOpenTrace;
		bool isEmpty;
	};
}  // namespace FeedTheDog

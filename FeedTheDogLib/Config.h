#pragma once
#include "ITraceConfig.h"
#include "TraceSource.h"
namespace FeedTheDog
{
	// TODO: ���ܴ����ƣ��ļ�����Ҫ�ܰ����ڷ�
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

#pragma once
#include "ITraceConfig.h"
#include "IThreadCount.h"
#include "TraceSource.h"
namespace FeedTheDog
{
	// TODO: ���ܴ����ƣ��ļ�����Ҫ�ܰ����ڷ�
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

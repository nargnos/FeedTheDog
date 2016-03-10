#pragma once
#include "TraceSource.h"
namespace FeedTheDog
{
	enum class LogMsg
	{
		None,
	};
	class Config
	{
	public:
		static const int BufferSize = 1024 * 10;
		Config();
		~Config();
		void Load();
		void Save();
		int GetThreadCount();
		int GetMaxThreadCount() const;
	private:
		int maxThreadCount;
		char* configPath;
		//TraceSource<LogMsg> kk;
		shared_ptr<Json::StreamWriter> writer;
		Json::Value root;
		Json::Value& ConfigNode();
		Json::Value& LoggerNode();

		Json::Value& ThreadCountMember();
	};
}  // namespace FeedTheDog

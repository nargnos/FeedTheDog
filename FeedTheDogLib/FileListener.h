#pragma once
#include "ListenerBase.h"
namespace FeedTheDog
{
	class FileListener :
		public ListenerBase
	{
	public:
		static Json::Value GetDefaultConfig();
		virtual ~FileListener() override;
		friend ListenerFactory;
	protected:
		virtual void WriteLine(const std::string &) override;
		FileListener(Json::Value & listenerConfig);
		static Json::Value& FileNameNode(Json::Value& listener);
		static  Json::Value& FlushIntervalNode(Json::Value& listener);
		unique_ptr<_STD ofstream> ofs;
		// Ë¢ÐÂËÙ¶È
		int flushSpeed;
		int count;
	};
}  // namespace FeedTheDog



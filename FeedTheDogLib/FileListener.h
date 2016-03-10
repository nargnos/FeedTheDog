#pragma once
#include "ListenerBase.h"
namespace FeedTheDog
{
	class FileListener :
		public ListenerBase
	{
	public:
		FileListener();
		~FileListener();


		// Í¨¹ý ListenerBase ¼Ì³Ð
		virtual void WriteLine(const std::string &) override;
		virtual void Init(Json::Value & listenerConfig) override;
	private:
		Json::Value& FileNameNode(Json::Value& listener);
		bool isOpen;
		shared_ptr<_STD ostream> ofs;
	};
}  // namespace FeedTheDog



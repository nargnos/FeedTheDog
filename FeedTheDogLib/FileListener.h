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


		// 通过 ListenerBase 继承
		virtual void WriteLine(const std::string &) override;
		virtual void Init(Json::Value & listenerConfig) override;
	private:
		Json::Value& FileNameNode(Json::Value& listener);
		bool isOpen;
		shared_ptr<_STD ostream> ofs;
		// 刷新速度
		int flushSpeed;
		int count;
	};
}  // namespace FeedTheDog



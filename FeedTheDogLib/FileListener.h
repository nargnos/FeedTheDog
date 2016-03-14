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


		// ͨ�� ListenerBase �̳�
		virtual void WriteLine(const std::string &) override;
		virtual void Init(Json::Value & listenerConfig) override;
	private:
		Json::Value& FileNameNode(Json::Value& listener);
		bool isOpen;
		shared_ptr<_STD ostream> ofs;
		// ˢ���ٶ�
		int flushSpeed;
		int count;
	};
}  // namespace FeedTheDog



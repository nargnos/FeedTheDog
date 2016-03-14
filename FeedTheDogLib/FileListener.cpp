#include "stdafx.h"
#include "FileListener.h"

namespace FeedTheDog
{
	FileListener::FileListener()
	{
		FileNameNode(defaultConfig) = ".\\log.log";
		count = 0;
		flushSpeed = 100;
	}


	FileListener::~FileListener()
	{
	}
	// WriteLine调用时已lock
	void FileListener::WriteLine(const std::string & str)
	{
		if (isOpen)
		{
			*ofs << str;
			count = (count + 1) % flushSpeed;
			if (!count)
			{
				ofs->flush();
			}
		}
	}
	void FileListener::Init(Json::Value & listenerConfig)
	{
		ListenerBase::Init(listenerConfig);
		// 设置文件名
		auto& fileNameNode = FileNameNode(listenerConfig);
		if (!fileNameNode.isString())
		{
			fileNameNode = FileNameNode(defaultConfig);
		}
		ofs = make_shared<_STD ofstream>(fileNameNode.asString(), _STD ios::app | _STD ios::out);
		isOpen = ofs->good();
	}
	Json::Value & FileListener::FileNameNode(Json::Value & listener)
	{
		return listener["FileName"];
	}
}  // namespace FeedTheDog


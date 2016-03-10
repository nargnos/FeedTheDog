#include "stdafx.h"
#include "FileListener.h"

namespace FeedTheDog
{
	FileListener::FileListener()
	{

	}


	FileListener::~FileListener()
	{
	}
	void FileListener::WriteLine(const std::string & str)
	{
		if (isOpen)
		{
			*ofs << str;
			ofs->flush();
		}
	}
	void FileListener::Init(Json::Value & listenerConfig)
	{
		ListenerBase::Init(listenerConfig);
		// 设置文件名
		auto& fileNameNode = FileNameNode(listenerConfig);
		if (!fileNameNode.isString())
		{
			fileNameNode = ".\\log.log";
		}
		ofs = make_shared<_STD ofstream>(fileNameNode.asString());
		isOpen = ofs->good();
	}
	Json::Value & FileListener::FileNameNode(Json::Value & listener)
	{
		return listener["FileName"];
	}
}  // namespace FeedTheDog


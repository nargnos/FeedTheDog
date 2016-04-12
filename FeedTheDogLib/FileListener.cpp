#include "stdafx.h"
#include "FileListener.h"

namespace FeedTheDog
{
	FileListener::FileListener(Json::Value & listenerConfig) :
		ListenerBase(listenerConfig)
	{
		auto defaultConfig = GetDefaultConfig();
		count = 0;

		auto& flushIntervalNode = FlushIntervalNode(listenerConfig);
		if (!flushIntervalNode.isInt())
		{
			flushIntervalNode = FlushIntervalNode(defaultConfig);
		}
		flushSpeed = flushIntervalNode.asInt();

		// �����ļ���
		auto& fileNameNode = FileNameNode(listenerConfig);
		if (!fileNameNode.isString())
		{
			fileNameNode = FileNameNode(defaultConfig);
		}
		ofs = make_unique<_STD ofstream>(fileNameNode.asString(), _STD ios::app | _STD ios::out);

	}


	Json::Value FileListener::GetDefaultConfig()
	{
		Json::Value result = ListenerBase::GetDefaultConfig();
		FileNameNode(result) = ".\\log.log";
		FlushIntervalNode(result) = 100;
		return result;
	}

	FileListener::~FileListener()
	{
		if (ofs->is_open())
		{
			ofs->close();
		}
	}
	// WriteLine����ʱ��lock
	void FileListener::WriteLine(const std::string & str)
	{
		if (ofs->is_open())
		{
			*ofs << str;
			if (flushSpeed < 0)
			{
				return;
			}
			count = (count + 1) % flushSpeed;
			if (!count)
			{
				ofs->flush();
			}
		}
	}
	Json::Value & FileListener::FileNameNode(Json::Value & listener)
	{
		return listener["FileName"];
	}
	Json::Value & FileListener::FlushIntervalNode(Json::Value & listener)
	{
		return listener["FlushInterval"];
	}
}  // namespace FeedTheDog


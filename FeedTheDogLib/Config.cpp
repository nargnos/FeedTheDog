#include "stdafx.h"
#include "Config.h"
namespace FeedTheDog
{
	Config::Config()
	{
		configPath = ".\\config.json";
		maxThreadCount = _BOOST thread::hardware_concurrency() * 2;
		Json::StreamWriterBuilder sb;		
		sb["indentation"] = "";
		sb["dropNullPlaceholders"] = true;
		writer = shared_ptr<Json::StreamWriter>(sb.newStreamWriter());
	}


	Config::~Config()
	{
		this->Save();
	}
	void Config::Load()
	{
		_STD ifstream ifs(configPath);
		if (!ifs.is_open())
		{
			ifs.close();
			return;
		}
		Json::Reader reader;
		Json::Value config;
		if (!reader.parse(ifs, config))
		{
			return;
		}
		root = config;
	}
	void Config::Save()
	{
		_STD ofstream ofs(configPath);
		writer->write(root, &ofs);
		ofs.close();
	}
	int Config::GetThreadCount()
	{
		auto result = ThreadCountMember().asInt();
		return result == 0 ? maxThreadCount : result;
	}
	int Config::GetMaxThreadCount() const
	{
		return maxThreadCount;
	}
	Json::Value & Config::ConfigNode()
	{
		return root["Config"];
	}
	Json::Value & Config::LoggerNode()
	{
		return root["Logger"];
	}
	Json::Value & Config::ThreadCountMember()
	{
		static bool isVerify = false;
		auto& threadCount = ConfigNode()["ThreadCount"];
		if (!isVerify)
		{
			if (threadCount.isInt())
			{
				auto count = threadCount.asInt();
				if (count < 0 || count > maxThreadCount)
				{
					threadCount = 0;
				}
			}
			else
			{
				threadCount = 0;
			}
			isVerify = true;
		}
		return threadCount;
	}
}  // namespace FeedTheDog




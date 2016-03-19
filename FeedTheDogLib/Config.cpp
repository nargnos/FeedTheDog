#include "stdafx.h"
#include "Config.h"

namespace FeedTheDog
{
	Config::Config()
	{
		configPath = ".\\config.json";
		maxThreadCount = _BOOST thread::hardware_concurrency();
		Json::StreamWriterBuilder sb;
		// 格式化配置信息
		// sb["indentation"] = "";
		// sb["dropNullPlaceholders"] = true;

		// 初始化对象
		trace = make_shared<TraceSource<TEnum>>();
		writer = shared_ptr<Json::StreamWriter>(sb.newStreamWriter());
		// 设置对应事件
		texts = make_shared<typename TraceSource<TEnum>::TMap>();		
		typedef typename TraceSource<TEnum>::TMap::value_type TPair;
		texts->insert(TPair(TEnum::Initialized, "Core Initialized"));
		texts->insert(TPair(TEnum::NewCore, "New Core"));
		texts->insert(TPair(TEnum::FreeCore, "Free Core"));
		texts->insert(TPair(TEnum::NewWorker, "New Worker"));
		texts->insert(TPair(TEnum::FreeWorker, "Free Worker"));
		texts->insert(TPair(TEnum::NewSessionPool, "New SessionPool"));
		texts->insert(TPair(TEnum::FreeSessionPool, "Free SessionPool"));
		texts->insert(TPair(TEnum::NewSession, "New Session"));
		texts->insert(TPair(TEnum::FreeSession, "Free Session"));
		texts->insert(TPair(TEnum::AllocMemory, "Alloc Memory"));
		texts->insert(TPair(TEnum::FreeMemory, "Free Memory"));
		texts->insert(TPair(TEnum::CoreStart, "Core Start"));
		texts->insert(TPair(TEnum::CoreStop, "=> Core Stop"));
		texts->insert(TPair(TEnum::StartWorker, "Start Worker"));
		texts->insert(TPair(TEnum::StopWorker, "Stop Worker"));
		texts->insert(TPair(TEnum::CloseAllSocket, "Close All Socket"));
		texts->insert(TPair(TEnum::MainEnd, "=> End Process"));
		texts->insert(TPair(TEnum::Exit, "Exit"));
		texts->insert(TPair(TEnum::AddService, "Add Service"));
		texts->insert(TPair(TEnum::DeleteService, "Delete Service"));

	}
	Config::~Config()
	{
		this->Save();
	}
	void Config::Load()
	{
		_STD ifstream ifs(configPath);
		if (ifs.is_open())
		{
			
			Json::Reader reader;
			Json::Value config;
			if (!reader.parse(ifs, config))
			{
				return;
			}
			root = config;
			ifs.close();
		}
		// 无配置文件，使用默认设置
		// 检查配置时，相应的字段为空的时候就表示需要使用默认设置，由各个类自己管理
		trace->Init(texts, ConfigNode()["Trace"]);
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
	shared_ptr<TraceSource<Config::TEnum>>& Config::GetTrace()
	{
		return trace;
	}
	Json::Value & Config::ConfigNode()
	{
		return root["Config"];
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




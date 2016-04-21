#include "stdafx.h"
#include "Config.h"
#include "ListenerFactory.h"
namespace FeedTheDog
{
	Config::Config()
	{
		configPath = ".\\config.json";
		Json::StreamWriterBuilder sb;
		// 格式化配置信息,紧凑输出
		// sb["indentation"] = "";
		// sb["dropNullPlaceholders"] = true;

		// 初始化对象
		writer = shared_ptr<Json::StreamWriter>(sb.newStreamWriter());

		_STD ifstream ifs(configPath);
		if (ifs.is_open())
		{

			Json::Reader reader;
			if (!reader.parse(ifs, root))
			{
				root.clear();
			}

			ifs.close();
		}

		isEmpty = root.empty();

		// 读取跟踪开关
		auto& openTraceNode = OpenTraceNode();
		this->isOpenTrace = openTraceNode.asBool();
	}

	Config::~Config()
	{
		this->Save();
	}

	void Config::Save()
	{
		_STD ofstream ofs(configPath);
		writer->write(root, &ofs);
		ofs.close();
	}

	Json::Value & Config::ConfigNode()
	{
		auto& result = root["Config"];
		if (!result.isObject())
		{
			result.swap(Json::Value());
		}
		return result;
	}
	Json::Value & Config::TraceNode()
	{
		auto& result = ConfigNode()["Trace"];
		if (!result.isObject())
		{
			result.swap(Json::Value());
		}
		return result;
	}
	Json::Value & Config::OpenTraceNode()
	{
		auto& result = TraceNode()["IsOpenTrace"];
		if (!result.isBool())
		{
			result = Json::Value(false);
		}
		return result;
	}

	bool Config::IsOpenTrace() const
	{
		return isOpenTrace;
	}
	void SetTraceComment(Json::Value & commentNode)
	{
		auto place = Json::CommentPlacement::commentAfter;
		// 添加配置注释
		if (commentNode.hasComment(place))
		{
			return;
		}
		auto& regs = ListenerFactory::GetListenerRegister();
		Json::Value commentJson;
		Json::Value& traceNode = commentJson["Config"]["Trace"];
		for each (auto& var in regs)
		{
			traceNode[var.first] = var.second.second();
		}
		ostringstream comment;
		comment << "/*===================" << _STD endl << commentJson.toStyledString() << "===================*/" << _STD endl;

		commentNode.setComment(comment.str(), place);
	}
	Config::TListenerVector Config::CreateListeners()
	{
		if (isEmpty)
		{
			SetTraceComment(root);
		}
		TListenerVector result;
		if (isOpenTrace)
		{
			auto& node = TraceNode();
			result.swap(ListenerFactory::CreateListeners(node, isEmpty));
		}
		return result;
	}
}  // namespace FeedTheDog




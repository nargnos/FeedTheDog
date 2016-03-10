#pragma once
#include "TraceSourceBase.h"
#include "ConsoleListener.h"
#include "FileListener.h"
#include "DebugListener.h"
namespace FeedTheDog
{
	// "OpenTrace":false,
	// "Listener":
	// {
	// 	"Console":
	// 	{
	// 		"TraceLevel":["trace", "debug", "info", "warning", "error", "fatal"]
	// 	},
	// 	"File":
	// 	{
	// 		"FileName":"log.log",
	// 		"TraceLevel":["trace", "debug", "info", "warning", "error", "fatal"],
	// 	},
	// 	"Debug":
	// 	{
	// 		"TraceLevel":["trace", "debug", "info", "warning", "error", "fatal"]
	// 	}
	// }
	template<typename TEnum>
	class TraceSource :
		public TraceSourceBase
	{
	public:
		typedef concurrent_unordered_map<TEnum, _STD string> TMap;
		TraceSource()
		{

		}
		void Init(const shared_ptr<TMap>& strs, Json::Value& config)
		{
			strmap = strs;
			// ∂¡»°∏˙◊Ÿø™πÿ
			auto& openTraceNode = config["OpenTrace"];
			if (!openTraceNode.isBool())
			{
				openTraceNode = false;
			}
			this->openTrace = openTraceNode.asBool();
			// …Ë÷√º‡Ã˝∆˜
			auto& listener = config["Listener"];
			ReadListener<ConsoleListener, "Console">(listener);
			ReadListener<FileListener, "File">(listener);
			ReadListener<DebugListener, "Debug">(listener);

		}
		template<typename TListener, const char* fieldName>
		void ReadListener(Json::Value& listener)
		{
			auto& node = listener[fieldName];
			if (!node)
			{
				auto tmp = make_shared<TListener>();
				tmp->Init(node);
				listeners.push_back(tmp);
			}
		}

		void TracePoint(TEnum msg, int i, TraceLevel level = TraceLevel::Debug)
		{
			if (!openTrace)
			{
				return;
			}
			mutex.lock();
			stringBuilder.clear();
			stringBuilder << *strmap[msg] << i << _STD endl;
			auto& str = stringBuilder.str();
			mutex.unlock();
			WriteLine(str);
		}
	private:
		shared_ptr<TMap> strmap;
		_STD ostringstream stringBuilder;

	};

}  // namespace FeedTheDog

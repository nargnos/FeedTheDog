#pragma once
#include "TraceSourceBase.h"
#include "ConsoleListener.h"
#include "FileListener.h"
#include "DebugListener.h"
#include "TraceDefine.h"
namespace FeedTheDog
{
#ifdef _DEBUG
//#define OPEN_DEBUGPOINT
#endif // _DEBUG

	template<typename TEnum>
	class TraceSource :
		public TraceSourceBase
	{
	public:

		typedef _STD unordered_map<TEnum, const _STD string> TMap;
		TraceSource()
		{
		}
		void Init(const shared_ptr<TMap> strs, Json::Value& config)
		{
			bool needSetDefaultCofig = config.empty();
			strmap = strs;
			// 读取跟踪开关
			auto& openTraceNode = config["OpenTrace"];
			if (!openTraceNode.isBool())
			{
				openTraceNode = false;
			}
			this->openTrace = openTraceNode.asBool();
			// 设置监听器
			auto& listener = config["Listener"];

			// 添加监听器
			ReadListener<ConsoleListener>(listener, "Console", needSetDefaultCofig);
			ReadListener<FileListener>(listener, "File", needSetDefaultCofig);
			ReadListener<DebugListener>(listener, "Debug", needSetDefaultCofig);

		}
		void DebugPoint(const _STD string& msg, bool useIndex = false, int index = 0, const char* str = NULL)
		{
#ifdef OPEN_DEBUGPOINT
			TracePoint(msg, useIndex, index, str, TraceLevel::Debug);
#endif // OPEN_DEBUGPOINT
		}
		void DebugPoint(TEnum msg, bool useIndex = false, int index = 0, const char* str = NULL)
		{
#ifdef OPEN_DEBUGPOINT
			TracePoint(msg, useIndex, index, str, TraceLevel::Debug);
#endif // OPEN_DEBUGPOINT
		}

		void TracePoint(const _STD string& msg, bool useIndex = false, int index = 0, const char* str = NULL, TraceLevel level = TraceLevel::Debug)
		{
			if (!openTrace)
			{
				return;
			}
			auto time = _STD chrono::system_clock::to_time_t(_STD chrono::system_clock::now());

			_STD ostringstream tmpSb;
			char tmpTimeBuffer[TimeBufferSize];
			auto result = ctime_s(tmpTimeBuffer, TimeBufferSize, &time);
			// 去掉后面的回车
			tmpTimeBuffer[strnlen_s(tmpTimeBuffer, TimeBufferSize) - 1] = 0;
			assert(!result);
			tmpSb << "[" << tmpTimeBuffer << "][" << TraceLevelString[(int)level] << "]\t" << msg;
			if (useIndex)
			{
				tmpSb << " (" << index << ")";
			}
			if (str != NULL)
			{
				tmpSb << " (" << str << ")";
			}
			tmpSb << _STD endl;

			auto& output = tmpSb.str();

			WriteLine(output, level);
		}
		void TracePoint(TEnum msg, bool useIndex = false, int index = 0, const char* str = NULL, TraceLevel level = TraceLevel::Debug)
		{
			if (!openTrace)
			{
				return;
			}
			TracePoint((*strmap)[msg], useIndex, index, str, level);
		}
		void TracePoint(const _STD string& msg, TraceLevel level)
		{
			TracePoint(msg, false, 0, NULL, level);
		}
		void TracePoint(TEnum msg, TraceLevel level)
		{
			TracePoint(msg, false, 0, NULL, level);
		}
	private:

		shared_ptr<TMap> strmap;
		template<typename TListener>
		void ReadListener(Json::Value& listener, const char* fieldName, bool setDefaultConfig = false)
		{
			auto& node = listener[fieldName];
			auto tmp = make_shared<TListener>();
			if (setDefaultConfig)
			{
				node = tmp->GetDefaultConfig();
			}
			if (openTrace && !node.empty())
			{
				// 为空时不创建Listener
				tmp->Init(node);
				listeners.push_back(tmp);
			}
		}
	};

}  // namespace FeedTheDog

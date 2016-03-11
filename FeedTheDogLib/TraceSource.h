#pragma once
#include "TraceSourceBase.h"
#include "ConsoleListener.h"
#include "FileListener.h"
#include "DebugListener.h"
#include "TraceDefine.h"
namespace FeedTheDog
{

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

			ReadListener<ConsoleListener>(listener, "Console", needSetDefaultCofig);
			ReadListener<FileListener>(listener, "File", needSetDefaultCofig);
			ReadListener<DebugListener>(listener, "Debug", needSetDefaultCofig);

		}

		void TracePoint(const _STD string& msg, bool useIndex = false, int index = 0, const char* str = NULL, TraceLevel level = TraceLevel::Debug)
		{
			if (!openTrace)
			{
				return;
			}
			auto time = _STD chrono::system_clock::to_time_t(_STD chrono::system_clock::now());
			mutex.lock();
			stringBuilder.str("");
			auto result = ctime_s(timeBuffer, TimeBufferSize, &time);
			// 去掉后面的回车
			timeBuffer[strnlen_s(timeBuffer, TimeBufferSize) - 1] = 0;
			assert(!result);
			stringBuilder << "[" << timeBuffer << "][" << TraceLevelString[(int)level] << "]\t" << msg;
			if (useIndex)
			{
				stringBuilder << " (" << index << ")";
			}
			if (str != NULL)
			{
				stringBuilder << " (" << str << ")";
			}
			stringBuilder << _STD endl;
			auto& output = stringBuilder.str();
			mutex.unlock();
			WriteLine(output, level);
		}
		void TracePoint(TEnum msg, bool useIndex = false, int index = 0, const char* str = NULL, TraceLevel level = TraceLevel::Debug)
		{
			if (!openTrace)
			{
				return;
			}
			TracePoint((*strmap)[msg], useIndex, index,str, level);
		}
	private:
		shared_ptr<TMap> strmap;
		_STD ostringstream stringBuilder;
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

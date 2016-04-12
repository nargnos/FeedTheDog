#pragma once

namespace FeedTheDog
{
#ifdef _DEBUG
//#define OPEN_DEBUGPOINT
#endif // _DEBUG
	template<typename Trait>
	class TraceSourceImpl
	{
	public:
		typedef typename Trait::TTraceLevel TTraceLevel;
		typedef typename TTraceLevel::Level TLevel;
		typedef typename Trait::TIListener TIListener;

		typedef typename Trait::TTraceConfig TTraceConfig;
		typedef typename TTraceConfig::TListenerVector TListenerVector;
		typedef typename Trait::TTraceFormat TTraceFormat;

		TraceSourceImpl(TTraceConfig& config):
			openTrace(config.IsOpenTrace()),
			listeners(config.CreateListeners())
		{

		}
		
		void __fastcall DebugPoint(const char* msg, bool useIndex = false, int index = 0, const char* str = NULL)
		{
#ifdef OPEN_DEBUGPOINT
			TracePoint(msg, useIndex, index, str, TLevel::Debug);
#endif // OPEN_DEBUGPOINT
		}

		void __fastcall TracePoint(const char* msg, bool useIndex = false, int index = 0, const char* str = NULL, TLevel level = TLevel::Debug)
		{
			if (!openTrace)
			{
				return;
			}
			auto msgStr = TTraceFormat::Format(msg, useIndex, index, str, level).str();
			WriteLine(msgStr, level);
		}

		void __fastcall TracePoint(const char* msg, TLevel level)
		{
			TracePoint(msg, false, 0, NULL, level);
		}

		bool IsOpenTrace() const
		{
			return openTrace;
		}
		
	private:
		bool openTrace;
		TListenerVector listeners;
		void WriteLine(string & str, TLevel level)
		{
			for each (auto& var in listeners)
			{
				var->WriteLine(str, level);
			}
		}
		
	};

}  // namespace FeedTheDog

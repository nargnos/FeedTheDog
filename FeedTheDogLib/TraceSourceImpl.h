#pragma once

namespace FeedTheDog
{
#ifdef _DEBUG
	//#define OPEN_DEBUGPOINT
#endif // _DEBUG
	template<typename TTracePolicy>
	class TraceSourceImpl
	{
	public:
		typedef typename TTracePolicy::TTraceLevel TTraceLevel;
		typedef typename TTracePolicy::TLevel TLevel;
		typedef typename TTracePolicy::TIListener TIListener;

		typedef typename TTracePolicy::TTraceConfig TTraceConfig;
		typedef typename TTracePolicy::TListenerVector TListenerVector;
		typedef typename TTracePolicy::TTraceFormat TTraceFormat;

		TraceSourceImpl(TTraceConfig& config) :
			openTrace(config.IsOpenTrace()),
			listeners(config.CreateListeners())
		{
			for (auto i = (int)TLevel::_Begin; i < (int)TLevel::_End; i++)
			{
				for each (auto& var in listeners)
				{
					visibleLevel[i] |= var->CheckLevel(static_cast<TLevel>(i));
				}
			}

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
			if (!visibleLevel[static_cast<int>(level)])
			{
				return;
			}
			auto msgStr = TTraceFormat::Format(msg, useIndex, index, str, level).str();
			TTracePolicy::WriteLine(listeners, msgStr, level);
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
		bool visibleLevel[(int)TLevel::_End];
		TListenerVector listeners;
	};

}  // namespace FeedTheDog

#pragma once

namespace FeedTheDog
{
	template<typename TTraceLevel>
	class TraceFormat
	{
	public:
		static ostringstream __fastcall Format(const char* msg, bool useIndex, int index, const char* str,typename TTraceLevel::Level level)
		{
			auto time = _STD chrono::system_clock::to_time_t(_STD chrono::system_clock::now());

			ostringstream result;
			char tmpTimeBuffer[TimeBufferSize];
			auto err=ctime_s(tmpTimeBuffer, TimeBufferSize, &time);
			// 去掉后面的回车
			tmpTimeBuffer[strnlen_s(tmpTimeBuffer, TimeBufferSize) - 1] = 0;
			
			result << "[" << tmpTimeBuffer << "][" << TTraceLevel::Parse(level) << "]\t" << msg;
			if (useIndex)
			{
				result << " (" << index << ")";
			}
			if (str != NULL)
			{
				result << " (" << str << ")";
			}
			result << _STD endl;

			return result;
		}

		~TraceFormat() = default;
	private:
		static const int TimeBufferSize = 64;
		TraceFormat() = default;
	};
}  // namespace FeedTheDog


#pragma once
namespace FeedTheDog
{
	class TraceLevel
	{
	public:
		enum class Level :unsigned char
		{
			_Begin = 0,
			Trace = _Begin,
			Debug,
			Info,
			Warning,
			Error,
			Fatal,
			_End,
		};

		static bool TryParse(string& level, TraceLevel::Level* result);
		static bool TryParse(TraceLevel::Level level, const char** result);

		static const char* Parse(TraceLevel::Level level);
		inline static const char* ConvertToStr(char level);
		inline static const char* ConvertToStr(TraceLevel::Level level);
	private:
		~TraceLevel() = delete;

		TraceLevel() = delete;
		static const char* TraceLevelString[];
		inline static bool CheckLevel(char level);
	};

}  // namespace FeedTheDog

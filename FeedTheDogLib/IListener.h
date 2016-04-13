#pragma once
namespace FeedTheDog
{	
	template<typename TTraceLevel>
	struct IListener
	{
		typedef TTraceLevel TTraceLevel;
		virtual void WriteLine(const _STD string&, TTraceLevel)=0;
		virtual bool CheckLevel(TTraceLevel level) const =0;
		static Json::Value GetDefaultConfig()
		{
			assert(false);
			return Json::Value();
		};
	};
}  // namespace FeedTheDog


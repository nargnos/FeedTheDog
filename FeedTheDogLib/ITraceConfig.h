#pragma once

namespace FeedTheDog
{
	template<typename TIListener>
	struct NOVTABLE ITraceConfig
	{
		typedef _STD vector<unique_ptr<TIListener>> TListenerVector;
		virtual bool IsOpenTrace() const = 0;
		virtual TListenerVector CreateListeners() = 0;
	};
}  // namespace FeedTheDog

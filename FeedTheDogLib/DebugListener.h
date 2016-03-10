#pragma once
#include "ListenerBase.h"
namespace FeedTheDog
{
	class DebugListener :
		public ListenerBase
	{
	public:
		DebugListener();
		~DebugListener();

		// Í¨¹ý ListenerBase ¼Ì³Ð
		virtual void WriteLine(const std::string &) override;
	};

}  // namespace FeedTheDog


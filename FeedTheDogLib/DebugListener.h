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

		// ͨ�� ListenerBase �̳�
		virtual void WriteLine(const std::string &) override;
	};

}  // namespace FeedTheDog


#pragma once
#include "ListenerBase.h"
namespace FeedTheDog
{
	class ConsoleListener :
		public ListenerBase
	{
	public:
		ConsoleListener();
		~ConsoleListener();

		// ͨ�� ListenerBase �̳�
		virtual void WriteLine(const std::string &) override;
	};

}  // namespace FeedTheDog


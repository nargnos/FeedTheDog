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

		// Í¨¹ý ListenerBase ¼Ì³Ð
		virtual void WriteLine(const std::string &) override;
	};

}  // namespace FeedTheDog


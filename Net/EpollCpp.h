
#ifndef EPOLLCPP_H_
#define EPOLLCPP_H_
#include <cassert>
#include <array>
#include <sys/epoll.h>
#include "Noncopyable.h"
namespace Detail
{
	struct EpollOption;
	class EpollCpp final :
		public Noncopyable
	{
	public:
		using Events = std::array<epoll_event, 128>;

		EpollCpp();
		~EpollCpp();
		void Add(int fd, EpollOption op, const void* userdata) const;
		void Mod(int fd, EpollOption op, const void* userdata) const;
		void Del(int fd) const;

		size_t Wait(Events& e, int ms);
		static epoll_event CreateEpollEvent(EpollOption op, const void* userdata);
	private:
		// fd’˝»∑∆ı‘º
		void EpollCtl(int flag, int fd, const epoll_event& ev) const;
		int epfd_;
	};

}  // namespace Detail
#endif /* EPOLLCPP_H_ */

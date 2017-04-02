
#ifndef EPOLLOPTION_H_
#define EPOLLOPTION_H_
#include <stdint.h>
#include <sys/epoll.h>

struct EpollOption
{
	EpollOption(uint32_t event);
	EpollOption();
	union
	{
		uint32_t Value;
		// 没啥用，debug 时好看些
		struct
		{
			bool In : 1;
			bool Pri : 1;
			bool Out : 1;
			bool Err : 1;
			bool Hup : 1;
			bool : 1;
			bool : 1; // RdNorm
			bool : 1; // RdBand

			bool : 1; // WrNorm
			bool : 1; // WrBand
			bool : 1; // Msg
			bool : 1; // x800
			bool : 1; // x1000
			bool RdHup : 1;
			bool : 1; // x4000
			bool : 1; // x8000

			unsigned int : 8;

			bool : 1; // x 100 0000
			bool : 1; // x 200 0000
			bool : 1; // x 400 0000
			bool : 1; // x 800 0000
			bool : 1; // x1000 0000
			bool WakeUp : 1;
			bool OneShot : 1;
			bool Et : 1;
		} Flags;
	};
};

#endif /* EPOLLOPTION_H_ */

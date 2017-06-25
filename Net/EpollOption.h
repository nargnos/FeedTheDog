
#ifndef EPOLLOPTION_H_
#define EPOLLOPTION_H_
#include <stdint.h>
#include <sys/epoll.h>
namespace Detail
{
	struct EpollOption
	{
		constexpr EpollOption(uint32_t event) :
			Value(event)
		{

		}
		constexpr EpollOption() : Value(0)
		{
		}
		inline operator uint32_t() const
		{
			return Value;
		}
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

				unsigned char : 8;

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

}  // namespace Detail
#endif /* EPOLLOPTION_H_ */

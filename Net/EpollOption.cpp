
#include "EpollOption.h"
namespace Detail
{

	EpollOption::EpollOption(uint32_t event) :
		Value(event)
	{

	}

	constexpr EpollOption::EpollOption() :Value(0)
	{
	}

	EpollOption::operator uint32_t() const
	{
		return Value;
	}

}  // namespace Detail
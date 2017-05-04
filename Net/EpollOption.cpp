
#include "EpollOption.h"

EpollOption::EpollOption(uint32_t event) :
	Value(event)
{

}

EpollOption::EpollOption() :Value(0)
{
}

EpollOption::operator uint32_t() const
{
	return Value;
}

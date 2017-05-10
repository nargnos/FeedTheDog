#include "GetWorkersAttorney.h"
#include "IoService.h"
namespace Detail
{
	const std::vector<std::unique_ptr<Worker>>&
		GetWorkersAttorney::Workers(const IoService& ios)
	{
		return ios.Workers();
	}

}  // namespace Detail
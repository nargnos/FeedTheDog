#include "GetWorkersAttorney.h"
#include "IoService.h"
namespace Detail
{
	const std::vector<std::unique_ptr<Worker>>&
		GetWorkersAttorney::Workers(const IoService& ios)
	{
		return ios.Workers();
	}
	const std::unique_ptr<Worker>& GetWorkersAttorney::FirstWorker(const IoService& ios)
	{
		return ios.workers_.front();
	}
}  // namespace Detail
#ifndef GETWORKERSATTORNEY_H_
#define GETWORKERSATTORNEY_H_
#include <vector>
#include <memory>

namespace Detail
{
	class IoService;
	class Worker;
	class GetWorkersAttorney
	{
	public:
		static const std::vector<std::unique_ptr<Worker>>& Workers(const IoService&);
		static const std::unique_ptr<Worker>& FirstWorker(const IoService & ios);
	private:
		GetWorkersAttorney() = delete;
		~GetWorkersAttorney() = delete;
	};

}  // namespace Detail

#endif // GETWORKERSATTORNEY_H_


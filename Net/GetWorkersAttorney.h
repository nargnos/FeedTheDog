#ifndef GETWORKERSATTORNEY_H_
#define GETWORKERSATTORNEY_H_
#include <vector>
#include <memory>
class IoService;
class Worker;
class GetWorkersAttorney
{
public:
	static const std::vector<std::unique_ptr<Worker>>& Workers(const IoService&);
private:
	GetWorkersAttorney() = delete;
	~GetWorkersAttorney() = delete;
};


#endif // GETWORKERSATTORNEY_H_


#ifndef PROGRESS_H_
#define PROGRESS_H_
#include <vector>
#include <sys/socket.h>
namespace Detail
{
	class Progress
	{
	public:
		typedef std::vector<iovec>::iterator iterator;
		typedef std::vector<iovec>::const_iterator const_iterator;

		explicit Progress(std::vector<iovec>&& iovs);
		Progress(const_iterator begin, const_iterator end);
		~Progress() = default;
		iovec* Iovec() const;
		size_t Count() const;
		size_t Trans() const;
		// 返回还有几个buffer才能完成
		size_t Step(size_t trans);
		Progress& operator=(Progress&& p);
	private:
		std::vector<iovec> iovs_;
		iovec* beginIt_;
		size_t trans_;
		size_t count_;
	};
}  // namespace Detail

#endif // PROGRESS_H_


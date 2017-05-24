#include "Progress.h"
#include "Util.h"
Detail::Progress::Progress(std::vector<iovec>&& iovs) :
	iovs_(std::move(iovs)),
	beginIt_(&iovs_.front()),
	trans_(0),
	count_(iovs_.size())
{
}

Detail::Progress::Progress(const_iterator begin, const_iterator end) :
	iovs_(begin, end),
	beginIt_(&iovs_.front()),
	trans_(0),
	count_(iovs_.size())
{
}

iovec * Detail::Progress::Iovec() const
{
	return beginIt_;
}

size_t Detail::Progress::Count() const
{
	return count_;
}

size_t Detail::Progress::Trans() const
{
	return trans_;
}


size_t Detail::Progress::Step(size_t trans)
{
	trans_ += trans;
	do
	{
		auto& len = beginIt_->iov_len;
		if (LIKELY(trans < len))
		{
			len -= trans;
			beginIt_->iov_base = static_cast<char*>(beginIt_->iov_base) + trans;
			break;
		}
		else
		{
			++beginIt_;
			--count_;
			trans -= len;
		}
	} while (trans > 0);
	return Count();
}

Detail::Progress & Detail::Progress::operator=(Progress && p)
{
	if (this == &p)
	{
		return *this;
	}
	iovs_ = std::move(p.iovs_);
	beginIt_ = p.beginIt_;
	trans_ = p.trans_;
	count_ = p.count_;
	return *this;
}

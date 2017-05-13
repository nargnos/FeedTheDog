#ifndef BASEFROMMEMBER_H_
#define BASEFROMMEMBER_H_
#include <utility>
template<typename T, int TUniqID = 0>
class BaseFromMember
{
protected:
	template<typename... TArgs>
	BaseFromMember(TArgs&&... args) :
		member_(std::forward<TArgs>(args)...)
	{
	}
	BaseFromMember() = default;
	T& Member()
	{
		return member_;
	}
	const T& Member() const
	{
		return member_;
	}
private:
	T member_;
};


#endif // BASEFROMMEMBER_H_


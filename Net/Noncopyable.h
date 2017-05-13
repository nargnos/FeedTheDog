#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_
class Noncopyable
{
protected:
	constexpr Noncopyable() = default;
	~Noncopyable() = default;
	Noncopyable(const Noncopyable&) = delete;
	Noncopyable& operator=(const Noncopyable&) = delete;
};


#endif // !NONCOPYABLE_H_

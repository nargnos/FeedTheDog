#pragma once
struct NonCopyable
{
protected:
	constexpr NonCopyable() = default;
private:
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};
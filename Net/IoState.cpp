#include "IoState.h"
#include <cassert>

// 默认IO已准备好

IoState::IoState() :
	current_(IoStatus::IoReady)
{
}

IoState::IoState(char val) :
	val_(val)
{}

IoState::operator char() const
{
	return val_;
}

char IoState::Value() const
{
	return val_;
}

IoState::IoStatus IoState::State() const
{
	return current_;
}

bool IoState::HasError() const
{
	return current_ >= IoStatus::Error;
}

bool IoState::IsClose() const
{
	return  current_ >= IoStatus::Close;
}

bool IoState::IsGood() const
{
	// 见前面的表
	assert(current_ != IoStatus::None);
	return current_ <= IoStatus::DoIo;
}

void IoState::SetIoReady(bool val)
{
	val_ = (val_ & ~IoStatus::IoReady) | val;
}

void IoState::SetBuffReady(bool val)
{
	val_ = (val_ & ~IoStatus::BufReady) | (val << 1);
}

void IoState::SetClose()
{
	val_ |= IoStatus::Close;
}

void IoState::SetError()
{
	current_ = IoStatus::Error;
}

bool IoState::IsIoReady() const
{
	return (val_ & (IoStatus::IoReady | IoStatus::Close | IoStatus::Error)) == IoStatus::IoReady;
}

bool IoState::IsBuffReady() const
{
	return  (val_ & (IoStatus::BufReady | IoStatus::Close | IoStatus::Error)) == IoStatus::BufReady;
}

bool IoState::IsDoIo() const
{
	return (val_ & (IoStatus::DoIo | IoStatus::Close | IoStatus::Error)) == IoStatus::DoIo;
}

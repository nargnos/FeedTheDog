#pragma once
enum class EmitStatusType :char
{
	Break,
	Continue,
	Stop
};
struct EmitStatus
{
	constexpr EmitStatus(EmitStatusType val) :Status(val)
	{

	}
	constexpr EmitStatus(int val) : EmitStatus(val != 0)
	{

	}
	constexpr EmitStatus(bool val) :
		Status(val ? EmitStatusType::Continue : EmitStatusType::Stop)
	{

	}
	EmitStatusType Status;
};
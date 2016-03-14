#pragma once
#include "Trait.h"
#include "SessionPoolTrait.h"
namespace FeedTheDog
{
	struct ISessionPoolBase
	{
		virtual unsigned int GetSessionCount() const = 0;
		virtual void CloseAll() = 0;
		virtual void RemoveServiceSession(const char* serviceName) = 0;
	};
	template<typename TProtocol>
	struct ISessionPool :public ISessionPoolBase
	{
		virtual shared_ptr<typename SessionPoolTrait::TSession<TProtocol>::type> NewSession(const char* serviceName) = 0;
	};
}  // namespace FeedTheDog

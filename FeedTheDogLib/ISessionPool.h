#pragma once
#include "Trait.h"
#include "SessionPoolTrait.h"
namespace FeedTheDog
{
	//struct ISessionPoolBase
	//{
	//	virtual void __PreDestruct()=0;
	//	virtual unsigned int GetSessionCount() const = 0;
	//	virtual void CloseAll() = 0;
	//	virtual void RemoveServiceSession(const char* serviceName) = 0;
	//	virtual ~ISessionPoolBase()
	//	{

	//	}
	//};
	//template<typename TProtocol>
	//struct ISessionPool :public ISessionPoolBase
	//{
	//	typedef typename TProtocol::resolver TResolver;
	//	virtual TResolver& GetResolver()=0;
	//	virtual shared_ptr<typename SessionPoolTrait::TSession<TProtocol>::type> NewSession(const char* serviceName) = 0;
	//};
}  // namespace FeedTheDog

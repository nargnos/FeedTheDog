#pragma once
#include "Define.h"
#include "IAddon.h"
namespace FeedTheDog
{
	class ServiceBaseImpl;
	__interface IService
	{
		const char* Name() const;
		void Start();
		void Stop();
		bool Init(const shared_ptr<ServiceBaseImpl>&);
		bool AddAddon(shared_ptr<IAddon>&);
		void RemoveAddon(const char*);
	};
}  // namespace FeedTheDog

#pragma once
#include "MemoryPoolType.h"
namespace FeedTheDog
{
	//PoolTypeID::
	template<template<typename, typename...> class TPoolType>
	struct PoolTrait;

	template<>
	struct PoolTrait<_BOOST object_pool>
	{
		enum
		{
			Value = BoostObjectPool
		};
	};
	template<>
	struct PoolTrait<_BOOST pool>
	{
		enum
		{
			Value = BoostPool
		};
	};
}  // namespace FeedTheDog

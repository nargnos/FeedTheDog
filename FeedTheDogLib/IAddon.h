#pragma once
namespace FeedTheDog
{
	// TODO: 加密模块
	// 需要能够在传输中动态的添加移除
	__interface IAddon
	{
		const char* Name();
	};
}  // namespace FeedTheDog

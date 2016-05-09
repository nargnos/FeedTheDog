#pragma once
namespace FeedTheDog
{
	// ��Բ�ͬ����ִ��ͬһ�ֺ���
	template<
		typename...>
		struct ForEachTypes;

	template<
		typename THead,
		typename... TTails>
		struct ForEachTypes<THead, TTails...>
	{
		template<typename TFunc>
		static inline void Run(TFunc& func)
		{
			func.Do<THead>();
			ForEachTypes<TTails...>::Run(func);
		}
	};
	template<>
	struct ForEachTypes<>
	{
		template<typename TFunc>
		static inline void Run(TFunc&)
		{
		}
	};
}  // namespace FeedTheDog

#pragma once
namespace FeedTheDog
{
	template<typename TOwner>
	class Owner
	{
	public:
		typedef TOwner TOwner;
		Owner(TOwner* owner)
		{
			static_assert(_STD is_class<TOwner>::value,"Owner");
			owner_ = owner;
		}
		TOwner* GetOwner()
		{
			return owner_;
		}
	protected:
		TOwner* owner_;
	};

}  // namespace FeedTheDog

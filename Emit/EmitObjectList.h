#pragma once
#include <forward_list>
#include <memory>
#include <algorithm>
#include "IEmitDo.h"
namespace Detail
{
	class EmitObjectList
	{
	protected:
		using ObjectPtr = _STD shared_ptr<IEmitDo>;
		using ObjectList = _STD forward_list<ObjectPtr>;
	public:
		EmitObjectList() = default;

		virtual ~EmitObjectList() = default;

		template<typename TEmitDo, typename... TArgs>
		TEmitDo* CreateInstance(TArgs&&... args)
		{
			static_assert(_STD is_base_of<IEmitDo, TEmitDo>::value, "is_base_of error");
			auto obj = _STD make_shared<TEmitDo>(_STD forward<TArgs>(args)...);
			assert(obj);
			objects_.push_front(obj);
			return obj.get();
		}
		bool Contain(IEmitDo* ptr)
		{
			return _STD any_of(objects_.begin(), objects_.end(), [ptr](const _STD shared_ptr<IEmitDo>& val)
			{
				return val.get() == ptr;
			});
		}
	private:
		ObjectList objects_;
	};

}
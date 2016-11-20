#pragma once
#include <type_traits>
#include "EmitStatus.h"
namespace Detail
{

	__interface IEmitDo
	{
		EmitStatus Do() const;
	};
}
template<typename T, typename TResultType>
using CheckType = _STD enable_if_t<!_STD is_base_of<Detail::IEmitDo, _STD decay_t<T>>::value, TResultType>;

template<typename T>
using IsVoidResult = _STD is_void<_STD result_of_t<T()>>;

template<typename T>
using IsEmitStatusResult = _STD is_convertible<_STD result_of_t<T()>, EmitStatus>;

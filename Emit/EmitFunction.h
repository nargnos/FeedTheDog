#pragma once
#include <utility>
#include "IEmitDo.h"
#include "NonCopyable.h"
namespace Detail
{

	template<typename TFunc>
	class EmitFunction :
		public IEmitDo,
		public NonCopyable
	{
	public:
		explicit EmitFunction(TFunc&& func) :
			function_(_STD forward<TFunc>(func))
		{
		}

		virtual ~EmitFunction() = default;

		virtual EmitStatus Do() const override
		{
			return RunFunc(function_);
		}
	private:
		template<typename T>
		static _STD enable_if_t<IsEmitStatusResult<T>::value, EmitStatus>
			RunFunc(T& func)
		{
			return func();
		}
		template<typename T>
		static _STD enable_if_t<!IsEmitStatusResult<T>::value && IsVoidResult<T>::value, EmitStatus>
			RunFunc(T& func)
		{
			func();
			return EmitStatus(EmitStatusType::Continue);
		}

		TFunc function_;
	};
}
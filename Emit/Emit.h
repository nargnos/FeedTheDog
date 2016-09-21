#pragma once
#include <type_traits>
// 参数什么的就用lambda捕获吧
class Emit
{
public:
	static auto DoNothing()
	{
		return []()
		{};
	}

	template<typename T>
	static auto ConstValue(T&& val)
	{
		return [val]()
		{
			return val;
		};
	}
	// 忽略返回值
	template<typename... TFunc>
	static auto Combined(TFunc&&... funcs)
	{
		return [funcs...]()
		{
			auto ignore = {_Invoke(funcs)...};
		};
	}

	// 忽略返回值
	template<typename... TFunc>
	static auto Combined()
	{
		return []()
		{
			auto ignore = {_Invoke<TFunc>()...};
		};
	}
	// 类静态函数转函数对象
	template<typename TFunc>
	static auto Convert()
	{
		return []()
		{
			return TFunc::Run();
		};
	}

	template<typename TCondition, typename TTrue, typename TFalse>
	static auto If(TCondition&& condition, TTrue&& t, TFalse&& f)
	{
		return [=]()
		{
			return condition() ? t() : f();
		};
	}
private:
	template<typename TFunc>
	static bool _Invoke(TFunc&& func)
	{
		func();
		return true;
	}
	template<typename TFunc>
	static bool _Invoke()
	{
		TFunc::Run();
		return true;
	}
	Emit() = delete;

	~Emit() = delete;
};


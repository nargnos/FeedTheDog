#pragma once
#include <type_traits>
// ����ʲô�ľ���lambda�����
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
	// ���Է���ֵ
	template<typename... TFunc>
	static auto Combined(TFunc&&... funcs)
	{
		return [funcs...]()
		{
			auto ignore = {_Invoke(funcs)...};
		};
	}

	// ���Է���ֵ
	template<typename... TFunc>
	static auto Combined()
	{
		return []()
		{
			auto ignore = {_Invoke<TFunc>()...};
		};
	}
	// �ྲ̬����ת��������
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


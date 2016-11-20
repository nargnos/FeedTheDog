#pragma once
#include <type_traits>
#include <algorithm>

// TODO: �����caseװ�������ļ�ȥ
template<typename TCase, TCase TVal, typename TFunc>
class ConstSwitchCase :
	_STD integral_constant<TCase, TVal>
{
public:
	ConstSwitchCase(TFunc&& func) :
		func_(_STD forward<TFunc>(func))
	{

	}
	bool operator()(TCase val, _Inout_ bool& canRun) const
	{
		if (canRun && val == value)
		{
			func_();
			canRun = false;
			return true;
		}
		return false;
	}
private:
	TFunc func_;
};
template<typename TCase, typename TFunc>
class ValSwitchCase
{
public:
	ValSwitchCase(TCase val, TFunc&& func) :
		value_(val),
		func_(_STD forward<TFunc>(func))
	{

	}
	bool operator()(TCase val, _Inout_ bool& canRun) const
	{
		if (canRun && val == value_)
		{
			func_();
			canRun = false;
			return true;
		}
		return false;
	}
private:
	TCase value_;
	TFunc func_;
};
template<typename TCondition, typename TFunc>
class ConditionSwitchCase
{
public:
	ConditionSwitchCase(TCondition&& con, TFunc&& func) :
		con_(_STD forward<TCondition>(con)),
		func_(_STD forward<TFunc>(func))
	{

	}
	template<typename TCase>
	bool operator()(TCase val, _Inout_ bool& canRun) const
	{
		if (canRun && con_(val))
		{
			func_();
			canRun = false;
			return true;
		}
		return false;
	}
private:
	TCondition con_;
	TFunc func_;
};


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
		return [=]()
		{
			// NOTICE: �������鸳ֵ�Ǵ����ң���֪���淶�Ƿ��޶�������������ĳЩ���������ܻ�����˳�򲻶Ե����⣬��Ҫע����ԣ���ͬ
			bool ignore[] = { _Invoke(funcs)... };
		};
	}

	// ���Է���ֵ
	template<typename... TFunc>
	static auto Combined()
	{
		return []()
		{
			bool ignore[] = { _Invoke<TFunc>()... };
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
	template<typename TCondition, typename... TCases>
	static auto Switch(TCondition&& condition, TCases&&... cases)
	{
		return [=]()
		{
			auto val = condition();
			bool canRun = true;
			bool ignore[] = { cases(val,canRun)... };
		};
	}
	template<typename TCondition, typename TDefault, typename... TCases>
	static auto SwitchDefault(TCondition&& condition, TDefault&& def, TCases&&... cases)
	{
		using namespace std;
		return [=]()
		{
			auto val = condition();
			bool canRun = true;
			bool ignore[] = { cases(val,canRun)... };
			if (canRun == true)
			{
				def();
			}
		};
	}
	template<typename TCase, TCase TVal, typename TFunc>
	static auto Case(TFunc&& func)
	{
		return ConstSwitchCase<TCase, TVal, TFunc>(_STD forward<TFunc>(func));
	}
	template<typename TCase, typename TFunc>
	static auto Case(TCase&& val, TFunc&& func)
	{
		return ValSwitchCase<TCase, TFunc>(_STD forward<TCase>(val), _STD forward<TFunc>(func));
	}
	// �������ͻ����������ж��ȵ��ȵ�
	template<typename TCondition, typename TFunc>
	static auto RangeCase(TCondition&& con, TFunc&& func)
	{
		return ConditionSwitchCase<TCondition, TFunc>(_STD forward<TCondition>(con), _STD forward<TFunc>(func));
	}
	// TODO: for\while\��������lambda��

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
		TFunc::Do();
		return true;
	}

	Emit() = delete;

	~Emit() = delete;
};


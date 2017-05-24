#ifndef OPERATOR_H_
#define OPERATOR_H_
#include <cassert>
#include "Util.h"
// ��������ʱ����ִ��cb
// TODO: ���������ʾ������Ӧ���ṩ�꾡�Ĵ�����Ϣ
enum class ErrStat :char
{
	None,
	Success,
	Close,
	Error,

	Pause,
	Continue
};

struct Error
{
	int Errno;
	ErrStat State;
	explicit operator bool() const
	{
		return State == ErrStat::Success;
	}

};

namespace Detail
{
	void SetErrno(Error& err);

	void SetErrStat(Error& err, ErrStat s);
	bool IsContinue(int err);
	class Operator
	{
	public:
		using Do = void(*)(Operator*, Error&);
		using Complete = void(*)(Operator*, Error);
		constexpr Operator(Do op, Complete cp) :
			operator_(op),
			complete_(cp)
		{
		}
		void DoOperator(Error& error);
		void DoComplete(Error e);

		virtual ~Operator() = default;
	private:
		Do operator_;
		Complete complete_;
	};

	template<typename TChild>
	class OperatorImpl :
		public Operator
	{
	public:
		constexpr OperatorImpl() :
			Operator(DoOperator, DoComplete)
		{
		}
		static void DoComplete(Operator* ptr, Error e)
		{
			auto self = static_cast<TChild*>(ptr);
			self->OnComplete(e);
		}
		static void DoOperator(Operator* ptr, Error& e)
		{
			auto self = static_cast<TChild*>(ptr);
			self->OnOperator(e);
		}
	};

}  // namespace Detail
#endif // OPERATOR_H_


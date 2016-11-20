#pragma once
#include <functional>
#include <cassert>
#include <memory>
#include <list>

#include "EmitObjectList.h"
#include "IEmitDo.h"
#include "NonCopyable.h"
#include "EmitFunction.h"


namespace Detail
{

	class TaskBase
	{
	protected:
		using EmitPtr = IEmitDo*;
		using EmitRef = IEmitDo&;
		using FunctionList = _STD list<EmitPtr>;
	public:

		TaskBase() :
			functions_(_STD make_shared<FunctionList>()),
			objects_(_STD make_shared<EmitObjectList>())
		{}
		IEmitDo& CurrentNode()
		{
			assert(!GetFunctions().empty());
			return *GetFunctions().back();
		}
		virtual ~TaskBase() = default;
	protected:
		FunctionList& GetFunctions()
		{
			return *functions_;
		}
		const FunctionList& GetFunctions() const
		{
			return *functions_;
		}
		EmitObjectList& ObjectList()
		{
			return *objects_;
		}

		template<typename TContinueFunc>
		void AppendFunction(TContinueFunc&& func)
		{
			auto obj = ObjectList().CreateInstance<EmitFunction<TContinueFunc>>(_STD forward<TContinueFunc>(func));
			AppendPtr(obj);
		}
		void AppendPtr(IEmitDo* ptr)
		{
			assert(ObjectList().Contain(ptr));
			GetFunctions().push_back(ptr);
		}
		EmitStatusType RunFunctions() const
		{
			for (auto& item : GetFunctions())
			{
				auto stat = item->Do().Status;
				switch (stat)
				{
				case EmitStatusType::Continue:
					continue;
				case EmitStatusType::Stop:
					return EmitStatusType::Stop;
				case EmitStatusType::Break:
					// TODO: 
				default:
					return EmitStatusType::Continue;
				}
			}
			return EmitStatusType::Continue;
		}
	private:
		_STD shared_ptr<FunctionList> functions_;
		_STD shared_ptr<EmitObjectList> objects_;

	};

	struct InvokeDoRun
	{
		template<typename T>
		static void Invoke(T& ref)
		{
			ref.DoRun();
		}
	};

	template<typename TOwner>
	class TaskOwner
	{
	public:
		TaskOwner(TOwner& owner) :ownerPtr_(&owner)
		{

		}
	protected:
		TOwner& Owner()
		{
			assert(ownerPtr_ != nullptr);
			return *ownerPtr_;
		}
		TOwner& Owner() const
		{
			assert(ownerPtr_ != nullptr);
			return *ownerPtr_;
		}
	private:
		TOwner* ownerPtr_;
	};
	template<>
	class TaskOwner<void>
	{

	};


	template<typename TOwner>
	class Task :
		public TaskBase,
		public TaskOwner<TOwner>
	{
	public:
		using TCurrentOwner = _STD conditional_t<_STD is_void<TOwner>::value, Task, TOwner>;
		using TaskOwner::TaskOwner;
		template<typename TFunc>
		auto Then(TFunc&& func)
		{
			AppendFunction(_STD forward<TFunc>(func));
			return GetOwner<TOwner>();
		}
		template<typename TCondition>
		auto& If(TCondition&& condition)
		{
			// 需要传入自身类型、函数类型、tag
			auto obj = ObjectList().CreateInstance<EmitIf<TCurrentOwner, TCondition>>(_STD ref(GetOwner<TOwner>()), _STD forward<TCondition>(condition));
			AppendPtr(obj);
			return *obj;
		}
		void Run() const
		{
			DoRun();
		}
		friend InvokeDoRun;
	protected:
		template<typename TOwner_>
		_STD enable_if_t<_STD is_void<TOwner_>::value, Task&> GetOwner()
		{
			return *this;
		}

		template<typename TOwner_>
		_STD enable_if_t<!_STD is_void<TOwner_>::value, TOwner_&> GetOwner()
		{
			return Owner();
		}

		virtual void DoRun() const = 0;

	};
	template<typename TChild>
	using DefaultTask = Task<TChild>;

	template<typename TOwner>
	class EmitIfNode :
		public Task<EmitIfNode<TOwner>>,
		public IEmitDo
	{
	public:
		friend InvokeDoRun;
		EmitIfNode(TOwner& owner) :Task(*this), owner_(owner)
		{

		}

		virtual EmitStatus Do() const override
		{
			return RunFunctions();
		}
		template<typename TContinueFunc>
		auto& CaseTrue(TContinueFunc&& func)
		{
			return owner_.CaseTrue(_STD forward<TContinueFunc>(func));
		}
		template<typename TContinueFunc>
		auto& CaseFalse(TContinueFunc&& func)
		{
			return owner_.CaseFalse(_STD forward<TContinueFunc>(func));
		}
		auto& EndIf() const
		{
			return owner_.EndIf();
		}
	protected:
		virtual void DoRun() const override
		{
			InvokeDoRun::Invoke(owner_);
		}
		TOwner& owner_;
	};

	template<typename TOwner, typename TCondition>
	class EmitIf :
		public IEmitDo
	{
	public:
		friend InvokeDoRun;
		EmitIf(TOwner& owner, TCondition&& cdt) :
			cdt_(_STD forward<TCondition>(cdt)),
			isCompleted_(false),
			owner_(owner)
		{

		}
		~EmitIf()
		{
			assert(CheckCompleted());
		}
		virtual EmitStatus Do() const override
		{
			assert(CheckCompleted());
			if (InvokeConditionFunc())
			{
				return InvokeTrueFunc();
			}
			return InvokeFalseFunc();

		}
		template<typename TContinueFunc>
		auto& CaseTrue(TContinueFunc&& func)
		{
			SetTrue(_STD forward<TContinueFunc>(func));
			return *ifTrue_;
		}
		template<typename TContinueFunc>
		auto& CaseFalse(TContinueFunc&& func)
		{
			SetFalse(_STD forward<TContinueFunc>(func));
			return *ifFalse_;
		}

		auto& CaseTrue()
		{
			assert(!ifTrue_);
			ifTrue_ = MakeNode();
			return *ifTrue_;
		}
		auto& CaseFalse()
		{
			assert(!ifFalse_);
			ifFalse_ = MakeNode();
			return *ifFalse_;
		}
		TOwner& EndIf()
		{
			assert(ifTrue_ || ifFalse_);
			SetCompleted();
			return owner_;
		}
	protected:
		EmitStatus InvokeTrueFunc() const
		{
			if (ifTrue_)
			{
				return ifTrue_->Do();
			}
			return EmitStatusType::Continue;
		}
		EmitStatus InvokeFalseFunc() const
		{
			if (ifFalse_)
			{
				return ifFalse_->Do();
			}
			return EmitStatusType::Continue;
		}
		bool InvokeConditionFunc() const
		{
			return cdt_();
		}
		template<typename T>
		void SetTrue(T&& func)
		{
			assert(!ifTrue_);
			ifTrue_ = MakeNode();
			ifTrue_->Then(_STD forward<T>(func));
		}
		template<typename T>
		void SetFalse(T&& func)
		{
			assert(!ifFalse_);
			ifFalse_ = MakeNode();
			ifFalse_->Then(_STD forward<T>(func));
		}

		void SetCompleted()
		{
			isCompleted_ = true;
		}
		bool CheckCompleted() const
		{
			return isCompleted_;
		}
		_STD shared_ptr<EmitIfNode<EmitIf>> MakeNode()
		{
			return _STD make_shared<EmitIfNode<EmitIf>>(*this);
		}
		void DoRun() const
		{
			InvokeDoRun::Invoke(owner_);
		}
	private:
		TCondition cdt_;
		_STD shared_ptr<EmitIfNode<EmitIf>> ifTrue_;
		_STD shared_ptr<EmitIfNode<EmitIf>> ifFalse_;
		TOwner& owner_;
		bool isCompleted_;
	};



}

class EmitTask :
	public Detail::Task<void>
{
public:
	void operator()() const
	{
		Run();
	}
protected:
	virtual void DoRun() const override
	{
		RunFunctions();
	}
};



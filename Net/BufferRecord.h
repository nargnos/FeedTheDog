#ifndef BUFFERRECORD_H_
#define BUFFERRECORD_H_

#include <cassert>
#include <vector>
#include <tuple>
#include <memory>
#include "Noncopyable.h"
#include "Buffer.h"
#include "TransferProgress.h"
namespace Detail
{
	// 发生错误时立即执行cb
	// TODO: 这里错误提示不够，应该提供详尽的错误信息
	enum class Error :char
	{
		Success,
		Close,
		Error
	};

	template<typename TConn>
	class ProgressRecord :
		public Noncopyable
	{
	public:
		using RunCompletePtr = void(*)(ProgressRecord<TConn>*, TConn&, Error);
		using TConnDoFunc = bool(TConn::*)(ProgressRecord&, TransferProgress&);
		using RunTConnDoFuncPtr = bool(*)(ProgressRecord<TConn>*, TConn&, TConnDoFunc);

		ProgressRecord(const RunCompletePtr comp, const RunTConnDoFuncPtr run) :
			onComplete_(comp),
			onRun_(run)
		{
		}
		// 可从外部传入read write处理方式或直接执行cb
		// TODO: 有没有其它方法改进这个
		// 可以分成DoRead，DoWrite，然后在函数中直接执行io对应函数
		// 不过当执行conn回调时是会用到DoWrite的，还是只能用策略，执行conn回调就忽略掉策略
		bool Do(TConn& conn, TConnDoFunc func)
		{
			return onRun_(this, conn, func);
		}
		void Complete(TConn& conn, Error e)
		{
			onComplete_(this, conn, e);
		}
		virtual ~ProgressRecord() = default;
	private:

		RunCompletePtr onComplete_;
		RunTConnDoFuncPtr onRun_;
	};

	template<typename TConn, typename TCallback>
	class RecordImpl :
		public ProgressRecord<TConn>
	{
	public:
		RecordImpl(Buffer&& buff, size_t begin, TCallback&& cb, bool isTransSome) :
			ProgressRecord<TConn>(RunComplete, Run),
			progress_(std::move(buff), begin, isTransSome),
			cb_(std::forward<TCallback>(cb))
		{

		}

	private:
		static bool Run(ProgressRecord<TConn>* self, TConn& conn, typename ProgressRecord<TConn>::TConnDoFunc func)
		{
			auto ptr = static_cast<RecordImpl*>(self);
			// FIX: 热区
			return (conn.*func)(*self, ptr->progress_);
		}
		static void RunComplete(ProgressRecord<TConn>* self, TConn& conn, Error e)
		{
			auto ptr = static_cast<RecordImpl*>(self);
			auto& buff = ptr->progress_.GetBuffer();
			ptr->cb_(conn, std::move(buff), e);
		}
		TransferProgress progress_;
		TCallback cb_;
	};

	// void(TConn*, Error)
	template<typename TConn, typename TCallback>
	class CallbackRecord :
		public ProgressRecord<TConn>
	{
	public:
		explicit CallbackRecord(TCallback&& cb) :
			ProgressRecord<TConn>(RunComplete, Run),
			cb_(std::forward<TCallback>(cb))
		{
		}

	private:
		static bool Run(ProgressRecord<TConn>* self, TConn& conn, typename ProgressRecord<TConn>::TConnDoFunc func)
		{
			RunComplete(self, conn, Error::Success);
			return true;
		}
		static void RunComplete(ProgressRecord<TConn>* self, TConn& conn, Error e)
		{
			auto ptr = static_cast<CallbackRecord*>(self);
			ptr->cb_(conn, e);
		}
		TCallback cb_;
	};

	template<typename TConn, typename TCallback>
	std::unique_ptr<ProgressRecord<TConn>> MakeRecord(Buffer&& buff, size_t begin, TCallback&& cb, bool isTransSome = false)
	{
		return std::make_unique<RecordImpl<TConn, TCallback>>(
			std::move(buff), begin, std::forward<TCallback>(cb), isTransSome);
	}
	template<typename TConn, typename TCallback>
	std::unique_ptr<ProgressRecord<TConn>> MakeRecord(TCallback&& cb)
	{
		return std::make_unique<CallbackRecord<TConn, TCallback>>(
			std::forward<TCallback>(cb));
	}

}  // namespace Detail
using Detail::Error;

#endif // BUFFERRECORD_H_


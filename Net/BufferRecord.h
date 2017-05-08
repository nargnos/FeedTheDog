#ifndef BUFFERRECORD_H_
#define BUFFERRECORD_H_

#include <cassert>
#include <vector>
#include <tuple>
#include <memory>
#include "Noncopyable.h"
#include "Buffer.h"
#include "TransferProgress.h"
// 发生错误时立即执行cb，此时可以直接用errno，就直接用枚举
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
	using RunCompletePtr = void(*)(ProgressRecord<TConn>*, TConn*, Error);
	using TConnDoFunc = bool(TConn::*)(ProgressRecord&, TransferProgress&);
	using RunTConnDoFuncPtr = bool(*)(ProgressRecord<TConn>*, TConn*, TConnDoFunc);

	constexpr ProgressRecord(const RunCompletePtr comp, const RunTConnDoFuncPtr run) :
		onComplete_(comp),
		onRun_(run)
	{
	}
	bool Do(TConn* conn, TConnDoFunc func)
	{
		return onRun_(this, conn, func);
	}
	void Complete(TConn* conn, Error e)
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
	static bool Run(ProgressRecord<TConn>* self, TConn* conn, typename ProgressRecord<TConn>::TConnDoFunc func)
	{
		auto ptr = static_cast<RecordImpl*>(self);
		return (conn->*func)(*self, ptr->progress_);
	}
	static void RunComplete(ProgressRecord<TConn>* self, TConn* conn, Error e)
	{
		auto ptr = static_cast<RecordImpl*>(self);
		auto& buff = ptr->progress_.GetBuffer();
		ptr->cb_(conn, std::move(buff), e);
	}
	TransferProgress progress_;
	TCallback cb_;
};

// void(TConn*,Error)
template<typename TConn, typename TCallback>
class CallbackRecord :
	public ProgressRecord<TConn>
{
public:
	CallbackRecord(TCallback&& cb) :
		ProgressRecord<TConn>(RunComplete, Run),
		cb_(std::forward<TCallback>(cb))
	{
	}

private:
	static bool Run(ProgressRecord<TConn>* self, TConn* conn, typename ProgressRecord<TConn>::TConnDoFunc func)
	{
		RunComplete(self, conn, Error::Success);
		return true;
	}
	static void RunComplete(ProgressRecord<TConn>* self, TConn* conn, Error e)
	{
		auto ptr = static_cast<CallbackRecord*>(self);
		ptr->cb_(conn, e);
	}
	TCallback cb_;
};

template<typename TConn, typename TCallback>
std::unique_ptr<ProgressRecord<TConn>> MakeRecord(Buffer&& buff, size_t begin, TCallback&& cb, bool isTransSome = false)
{
	return std::unique_ptr<ProgressRecord<TConn>>(
		new RecordImpl<TConn, TCallback>(std::move(buff), begin, std::forward<TCallback>(cb), isTransSome));
}
template<typename TConn, typename TCallback>
std::unique_ptr<ProgressRecord<TConn>> MakeRecord(TCallback&& cb)
{
	return std::unique_ptr<ProgressRecord<TConn>>(
		new CallbackRecord<TConn, TCallback>(std::forward<TCallback>(cb)));
}



#endif // BUFFERRECORD_H_


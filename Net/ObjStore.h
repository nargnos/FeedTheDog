#ifndef OBJSTORE_H_
#define OBJSTORE_H_
#include <memory>
#include <list>
#include "Noncopyable.h"
// 暂存智能指针对象，之后用pos删除
// tls但非线程安全
template<typename TStoreType>
class ObjStore final
{
public:
	using Ptr = std::shared_ptr<TStoreType>;
	using StoreList = std::list<Ptr>;
	using Pos = typename StoreList::iterator;
	static void Detach(Pos& it)
	{
		auto& store = GetStore();
		if (it != store.end())
		{
			store.erase(it);
			it = store.end();
		}
	}
	static Pos Attach(const Ptr& ptr)
	{
		auto& store = GetStore();
		return store.insert(store.begin(), ptr);
	}
private:
	static StoreList& GetStore()
	{
		thread_local static StoreList store;
		return store;
	}
	ObjStore() = delete;
	~ObjStore() = delete;
};
template<typename TStoreType>
class StorePos
{
private:
	using Store = ObjStore<TStoreType>;
	using Pos = typename Store::Pos;
public:
	StorePos() = default;
	virtual ~StorePos() = default;
protected:
	void SetPos(Pos&& pos)
	{
		pos_ = pos;
	}
	void Detach()
	{
		Store::Detach(pos_);
	}
private:
	Pos pos_;
};

#endif // OBJSTORE_H_


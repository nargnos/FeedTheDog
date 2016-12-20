#pragma once
#include <utility>
#include <memory>
#include <vector>
#include <cassert>
#include <tuple>
#include <lua.hpp>
#include "LuaState.h"

// TODO: 如何注册任何函数（不按定义）到里面
// 没有反射情况下如何注册类到里面
class LuaUtil
{
public:
	LuaUtil(bool openlibs = true, lua_Alloc f = nullptr, void *ud = nullptr);
	LuaUtil(const _STD shared_ptr<LuaState>& luaState);
	const _STD shared_ptr<LuaState>& State() const;
	_STD vector<char> CodeToBin(const char* code) const
	{
		_STD vector<char> result;
		auto ret = State()->LoadString_L(code);

		if (ret == LoadResults::Ok)
		{
			result = Dump(true);
			State()->Pop(1);
		}
		return result;
	}
	_STD vector<char> Dump(bool strip) const
	{
		_STD vector<char> result;
		result.reserve(0x100);
		if (State()->Dump(VectorWriter, &result, strip) != 0)
		{
			result.clear();
		}
		return result;
	}
	LoadResults LoadBin(const _STD vector<char>& bin) const
	{
		return State()->LoadBuffer_L(bin.data(), bin.size(), nullptr);
	}

	template<typename TType>
	TType AsType(int index) const;

	template<>
	lua_CFunction AsType<lua_CFunction>(int index) const
	{
		return State()->ToCFunction(index);
	}
	template<>
	bool AsType<bool>(int index) const
	{
		return State()->ToBoolean(index);
	}
	template<>
	lua_Integer AsType<lua_Integer>(int index) const
	{
		return State()->ToInteger(index);
	}
	template<>
	int AsType<int>(int index) const
	{
		return State()->ToInteger(index);
	}
	template<>
	lua_Number AsType<lua_Number>(int index) const
	{
		return State()->ToNumber(index);
	}
	template<>
	const char* AsType<const char*>(int index) const
	{
		return State()->ToString(index);
	}
	template<>
	lua_State * AsType<lua_State *>(int index) const
	{
		return State()->ToThread(index);
	}
	template<>
	nullptr_t AsType<nullptr_t>(int index) const
	{
		return nullptr;
	}

	// 从左到右顺序入栈
	template<typename TArg, typename... TArgs>
	void Push(TArg&& arg, TArgs&&... args) const
	{
		PushVal(_STD forward<TArg>(arg));
		Push(_STD forward<TArgs>(args)...);
	}
	// index为负数就从栈顶读到底，整数就相反
	// tuple的类型顺序是读取顺序
	template<typename TArg, typename... TArgs>
	_STD tuple<TArg, TArgs...> ToTuple(int index)
	{
		typedef _STD tuple<TArg, TArgs...> TResult;
		TResult result;
		if (index != 0)
		{
			ReadStack<TResult, 0, false>::Read<TArg, TArgs...>(*this, result, index, index > 0 ? 1 : -1);
		}
		return result;
	}

private:
	template<typename TTuple, int idx, bool breakLoop>
	struct ReadStack
	{
		template<typename TArg, typename... TArgs>
		inline static void Read(LuaUtil& state, TTuple& ret, int stackIdx, int step)
		{
			_STD get<idx>(ret) = state.AsType<TArg>(stackIdx);
			ReadStack<TTuple, idx + 1, idx + 1 == _STD tuple_size<TTuple>::value>::Read<TArgs...>(state, ret, stackIdx + step, step);
		}
	};
	template<typename TTuple, int idx>
	struct ReadStack<TTuple, idx, true>
	{
		template<typename... TArgs>
		inline static void Read(LuaUtil& state, TTuple& ret, int stackIdx, int step)
		{
		}
	};


	inline static void Push()
	{}
	void PushVal(bool val) const
	{
		State()->PushBoolean(val);
	}
	void PushVal(int val) const
	{
		State()->PushInteger(val);
	}
	void PushVal(lua_Integer val) const
	{
		State()->PushInteger(val);
	}
	void PushVal(nullptr_t) const
	{
		State()->PushNil();
	}
	void PushVal(lua_Number n) const
	{
		State()->PushNumber(n);
	}
	const char* PushVal(const char *s) const
	{
		return State()->PushString(s);
	}

	static int VectorWriter(lua_State *L,
		const void* p,
		size_t sz,
		void* ud)
	{
		using namespace std;
		assert(ud);
		// 会分多次调用
		auto vec = reinterpret_cast<vector<char>*>(ud);
		auto bin = reinterpret_cast<const char*>(p);
		for (size_t i = 0; i < sz; i++)
		{
			vec->push_back(bin[i]);
		}

		return 0;
	}

	_STD shared_ptr<LuaState> luaState_;
};


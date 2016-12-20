#pragma once
#include <memory>
class LuaState;
struct lua_State;
class LuaStateLib
{
public:
	explicit LuaStateLib(const _STD shared_ptr<LuaState>& luaState);
protected:
	lua_State* L() const;
private:
	_STD shared_ptr<LuaState> luaState_;
};

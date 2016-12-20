#include "LuaStateLib.h"
#include <lua.hpp>
#include "LuaState.h"
LuaStateLib::LuaStateLib(const std::shared_ptr<LuaState>& luaState) :
	luaState_(luaState)
{
}

lua_State * LuaStateLib::L() const
{
	return luaState_->RawPtr();
}

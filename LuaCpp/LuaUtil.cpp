#include "LuaUtil.h"

LuaUtil::LuaUtil(bool openlibs, lua_Alloc f, void * ud)
{
	if (f != nullptr && ud != nullptr)
	{
		luaState_ = _STD make_shared<LuaState>(f, ud);
	}
	else
	{
		luaState_ = _STD make_shared<LuaState>();
	}
	if (openlibs)
	{
		State()->OpenLibs_L();
	}
}

LuaUtil::LuaUtil(const std::shared_ptr<LuaState>& luaState) :luaState_(luaState)
{

}

const std::shared_ptr<LuaState>& LuaUtil::State() const
{
	return luaState_;
}

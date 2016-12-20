#include "LuaDebug.h"
#include <lua.hpp>
#include "LuaState.h"

lua_Hook LuaDebug::GetHook() const
{
	return lua_gethook(L());
}

int LuaDebug::GetHookCount() const
{
	return lua_gethookcount(L());
}

int LuaDebug::GetHookMask() const
{
	return lua_gethookmask(L());
}

int LuaDebug::GetInfo(const char * what, lua_Debug * ar) const
{
	return lua_getinfo(L(), what, ar);
}

const char * LuaDebug::GetLocal(const lua_Debug * ar, int n) const
{
	return lua_getlocal(L(), ar, n);
}

int LuaDebug::GetStack(int level, lua_Debug * ar) const
{
	return lua_getstack(L(), level, ar);
}

const char * LuaDebug::GetUpvalue(int funcindex, int n) const
{
	return lua_getupvalue(L(), funcindex, n);
}

void LuaDebug::SetHook(lua_Hook f, int mask, int count) const
{
	return lua_sethook(L(), f, mask, count);
}

const char * LuaDebug::SetLocal(const lua_Debug * ar, int n) const
{
	return lua_setlocal(L(), ar, n);
}

const char * LuaDebug::SetUpvalue(int funcindex, int n) const
{
	return lua_setupvalue(L(), funcindex, n);
}

void * LuaDebug::UpvalueID(int funcindex, int n) const
{
	return lua_upvalueid(L(), funcindex, n);
}

void LuaDebug::UpvalueJoin(int funcindex1, int n1, int funcindex2, int n2) const
{
	return lua_upvaluejoin(L(), funcindex1, n1, funcindex2, n2);
}

#pragma once
#include "LuaStateLib.h"
struct lua_Debug;
struct lua_State;
typedef void(*lua_Hook) (lua_State *L, lua_Debug *ar);

class LuaDebug :public LuaStateLib
{
public:
	using LuaStateLib::LuaStateLib;
	lua_Hook GetHook() const;
	int GetHookCount() const;
	int GetHookMask() const;
	int GetInfo(const char *what, lua_Debug *ar) const;
	const char* GetLocal(const lua_Debug *ar, int n) const;
	int GetStack(int level, lua_Debug *ar) const;
	const char* GetUpvalue(int funcindex, int n) const;
	void SetHook(lua_Hook f, int mask, int count) const;
	const char* SetLocal(const lua_Debug *ar, int n) const;
	const char* SetUpvalue(int funcindex, int n) const;
	void* UpvalueID(int funcindex, int n) const;
	void UpvalueJoin(int funcindex1, int n1, int funcindex2, int n2) const;
};
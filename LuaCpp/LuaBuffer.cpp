#include "LuaBuffer.h"
#include <lua.hpp>
#include "LuaState.h"
LuaBuffer::LuaBuffer() :
	buff_(_STD make_shared<luaL_Buffer>())
{

}

void LuaBuffer::AddChar_L(char c)
{
	luaL_addchar(Buff(), c);
}

void LuaBuffer::AddLString_L(const char * s, size_t l)
{
	return luaL_addlstring(Buff(), s, l);
}

void LuaBuffer::AddSize_L(size_t n)
{
	luaL_addsize(Buff(), n);
}

void LuaBuffer::AddString_L(const char * s)
{
	return luaL_addstring(Buff(), s);
}

void LuaBuffer::AddValue_L()
{
	return luaL_addvalue(Buff());
}

char * LuaBuffer::PrePBuffer_L()
{
	return luaL_prepbuffer(Buff());
}

char * LuaBuffer::PrePBuffSize_L(size_t sz)
{
	return luaL_prepbuffsize(Buff(), sz);
}

void LuaBuffer::PushResult_L()
{
	return luaL_pushresult(Buff());
}

void LuaBuffer::PushResultSize_L(size_t sz)
{
	return luaL_pushresultsize(Buff(), sz);
}

void LuaBuffer::BuffInit_L(LuaState & luaState)
{
	return luaL_buffinit(luaState.RawPtr(), Buff());
}

char * LuaBuffer::BuffInitSize_L(LuaState & luaState, size_t sz)
{
	return luaL_buffinitsize(luaState.RawPtr(), Buff(), sz);
}

luaL_Buffer * LuaBuffer::Buff() const
{
	return buff_.get();
}

#pragma once
#include <memory>
#include <lua.hpp>
struct luaL_Buffer;
class LuaState;
class LuaBuffer
{
public:
	explicit LuaBuffer();
	
	void AddChar_L(char c);
	void AddLString_L(const char *s, size_t l);
	void AddSize_L(size_t n);
	void AddString_L(const char *s);
	void AddValue_L();
	char * PrePBuffer_L();
	char * PrePBuffSize_L(size_t sz);
	void PushResult_L();
	void PushResultSize_L(size_t sz);
	void BuffInit_L(LuaState& luaState);
	char * BuffInitSize_L(LuaState& luaState, size_t sz);
private:
	_STD shared_ptr<luaL_Buffer> buff_;
	luaL_Buffer * Buff() const;
	
};


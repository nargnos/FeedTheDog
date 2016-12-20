#include "LuaState.h"
#include <cassert>
#include <lua.hpp>



LuaState::LuaState() :
	LuaState(LuaState::NewState_L(), true)
{
}

LuaState::LuaState(lua_Alloc f, void * ud) :
	LuaState(LuaState::NewState(f, ud), true)
{

}

LuaState::LuaState(lua_State * state, bool autoClose) :
	L(state),
	autoClose_(autoClose)
{

}


lua_State * LuaState::RawPtr() const
{
	return L;
}

int LuaState::AbsIndex(int index)
{

	return lua_absindex(L, index);
}

void LuaState::Arith(LuaOps op)
{
	lua_arith(L, static_cast<int>(op));
}

lua_CFunction LuaState::AtPanic(lua_CFunction panicf)
{
	return lua_atpanic(L, panicf);
}

void LuaState::Call(int nargs, int nresults)
{
	lua_call(L, nargs, nresults);
}

void LuaState::Callk(int nargs, int nresults, lua_KContext ctx, lua_KFunction k)
{
	lua_callk(L, nargs, nresults, ctx, k);
}

int LuaState::CheckStack(int n)
{
	return lua_checkstack(L, n);
}

void LuaState::Close()
{
	if (L != nullptr)
	{
		lua_close(L);
		L = nullptr;
	}
}

int LuaState::Compare(int left, int right, LuaCompareOps op)
{
	return lua_compare(L, left, right, static_cast<int>(op));
}

void LuaState::Concat(int n)
{
	lua_concat(L, n);
}

void LuaState::Copy(int fromIdx, int toIdx)
{
	lua_copy(L, fromIdx, toIdx);
}

void LuaState::CreateTable(int narr, int nrec)
{
	return lua_createtable(L, narr, nrec);
}

int LuaState::Dump(lua_Writer writer, void * data, int strip)
{
	return lua_dump(L, writer, data, strip);
}

int LuaState::Error()
{
	return lua_error(L);
}

int LuaState::Gc(GcOps what, int data)
{
	return lua_gc(L, static_cast<int>(what), data);
}

lua_Alloc LuaState::GetAllocF(void ** ud)
{
	return lua_getallocf(L, ud);
}

int LuaState::GetField(int index, const char * key)
{
	return lua_getfield(L, index, key);
}

void* LuaState::GetExtraSpace()
{
	return lua_getextraspace(L);
}

int LuaState::GetGlobal(const char * name)
{
	return lua_getglobal(L, name);
}

int LuaState::GetI(int index, lua_Integer i)
{
	return lua_geti(L, index, i);
}

int LuaState::GetMetaTable(int index)
{
	return lua_getmetatable(L, index);
}

int LuaState::GetTable(int index)
{
	return lua_gettable(L, index);
}

int LuaState::GetTop()
{
	return lua_gettop(L);
}

int LuaState::GetUserValue(int index)
{
	return lua_getuservalue(L, index);
}

void LuaState::Insert(int index)
{
	return lua_insert(L, index);
}

bool LuaState::IsBoolean(int index)
{
	return lua_isboolean(L, index) == 1;
}

bool LuaState::IsCFunction(int index)
{
	return lua_iscfunction(L, index) == 1;
}

bool LuaState::IsFunction(int index)
{
	return lua_isfunction(L, index) == 1;
}

bool LuaState::IsInteger(int index)
{
	return lua_isinteger(L, index) == 1;
}

bool LuaState::IsLightUserData(int index)
{
	return lua_islightuserdata(L, index) == 1;
}

bool LuaState::IsNil(int index)
{
	return lua_isnil(L, index) == 1;
}

bool LuaState::IsNone(int index)
{
	return lua_isnone(L, index) == 1;
}

bool LuaState::IsNoneOrNil(int index)
{
	return lua_isnoneornil(L, index) == 1;
}

bool LuaState::IsNumber(int index)
{
	return lua_isnumber(L, index) == 1;
}

bool LuaState::IsString(int index)
{
	return lua_isstring(L, index) == 1;
}

bool LuaState::IsTable(int index)
{
	return lua_istable(L, index) == 1;
}

bool LuaState::IsThread(int index)
{
	return lua_isthread(L, index) == 1;
}

bool LuaState::IsUserData(int index)
{
	return lua_isuserdata(L, index) == 1;
}

bool LuaState::IsYieldable()
{
	return lua_isyieldable(L) == 1;
}

void LuaState::Len(int index)
{
	return lua_len(L, index);
}

LoadResults LuaState::Load(lua_Reader reader, void * data, const char * chunkname, const char * mode)
{
	return static_cast<LoadResults>(lua_load(L,
		reader,
		data,
		chunkname,
		mode));
}

lua_State* LuaState::NewState(lua_Alloc f, void * ud)
{
	return lua_newstate(f, ud);
}

void LuaState::NewTable()
{
	lua_newtable(L);
}

lua_State* LuaState::NewThread()
{
	return lua_newthread(L);
}

void* LuaState::NewUserData(size_t size)
{
	return lua_newuserdata(L, size);
}

int LuaState::Next(int index)
{
	return lua_next(L, index);
}

int LuaState::NumberToInteger(lua_Number n, lua_Integer * p)
{
	return lua_numbertointeger(n, p);
}

CallResults LuaState::PCall(int nargs, int nresults, int msgh)
{
	return static_cast<CallResults>(lua_pcall(L, nargs, nresults, msgh));
}

CallResults LuaState::PCallK(int nargs, int nresults, int msgh, lua_KContext ctx, lua_KFunction k)
{
	return static_cast<CallResults>(lua_pcallk(L,
		nargs,
		nresults,
		msgh,
		ctx,
		k));
}

void LuaState::Pop(int n)
{
	lua_pop(L, n);
}

void LuaState::PushBoolean(bool val)
{
	lua_pushboolean(L, static_cast<int>(val));
}

void LuaState::PushCClosure(lua_CFunction fn, int n)
{
	lua_pushcclosure(L, fn, n);
}

void LuaState::PushCFunction(lua_CFunction f)
{
	lua_pushcfunction(L, f);
}

void LuaState::PushGlobalTable()
{
	lua_pushglobaltable(L);
}

void LuaState::PushInteger(lua_Integer val)
{
	lua_pushinteger(L, val);
}

void LuaState::PushLightUserData(void * p)
{
	lua_pushlightuserdata(L, p);
}

const char* LuaState::PushLString(const char * s, size_t len)
{
	return lua_pushlstring(L, s, len);
}

void LuaState::PushNil()
{
	lua_pushnil(L);
}

void LuaState::PushNumber(lua_Number n)
{
	lua_pushnumber(L, n);
}

const char* LuaState::PushString(const char * s)
{
	return lua_pushstring(L, s);
}

int LuaState::PushThread()
{
	return lua_pushthread(L);
}

void LuaState::PushValue(int index)
{
	lua_pushvalue(L, index);
}

const char* LuaState::PushVFString(const char * fmt, va_list argp)
{
	return lua_pushvfstring(L, fmt, argp);
}

int LuaState::RawEqual(int left, int right)
{
	return lua_rawequal(L, left, right) == 1;
}

int LuaState::RawGet(int index)
{
	return lua_rawget(L, index);
}

int LuaState::RawGetI(int index, lua_Integer n)
{
	return lua_rawgeti(L, index, n);
}

int LuaState::RawGetP(int index, const void * p)
{
	return lua_rawgetp(L, index, p);
}

size_t LuaState::RawLen(int index)
{
	return lua_rawlen(L, index);
}

void LuaState::RawSet(int index)
{
	lua_rawset(L, index);
}

void LuaState::RawSetI(int index, lua_Integer i)
{
	lua_rawseti(L, index, i);
}

void LuaState::RawSetP(int index, const void * p)
{
	lua_rawsetp(L, index, p);
}

void LuaState::Register(const char * name, lua_CFunction f)
{
	lua_register(L, name, f);
}

void LuaState::Remove(int index)
{
	lua_remove(L, index);
}

void LuaState::Replace(int index)
{
	lua_replace(L, index);
}

int LuaState::Resume(lua_State * from, int nargs)
{
	return lua_resume(L, from, nargs);
}

void LuaState::Rotate(int idx, int n)
{
	lua_rotate(L, idx, n);
}

void LuaState::SetAllocF(lua_Alloc f, void * ud)
{
	lua_setallocf(L, f, ud);
}

void LuaState::SetField(int index, const char * k)
{
	lua_setfield(L, index, k);
}

void LuaState::SetGlobal(const char * name)
{
	lua_setglobal(L, name);
}

void LuaState::SetI(int index, lua_Integer n)
{
	lua_seti(L, index, n);
}

int LuaState::SetMetaTable(int index)
{
	return lua_setmetatable(L, index);
}

void LuaState::SetTable(int index)
{
	return lua_settable(L, index);
}

void LuaState::SetTop(int index)
{
	return lua_settop(L, index);
}

void LuaState::SetUserValue(int index)
{
	return lua_setuservalue(L, index);
}

ThreadStatus LuaState::Status()
{
	return static_cast<ThreadStatus>(lua_status(L));
}

size_t LuaState::StringToNumber(const char * s)
{
	return lua_stringtonumber(L, s);
}

bool LuaState::ToBoolean(int index)
{
	return lua_toboolean(L, index) == 1;
}

lua_CFunction LuaState::ToCFunction(int index)
{
	return lua_tocfunction(L, index);
}

lua_Integer LuaState::ToInteger(int index)
{
	return lua_tointeger(L, index);
}

lua_Integer LuaState::ToIntegerx(int index, bool * isnum)
{
	return lua_tointegerx(L, index, reinterpret_cast<int*>(isnum));
}

const char* LuaState::ToLString(int index, size_t * len)
{
	return lua_tolstring(L, index, len);
}

lua_Number LuaState::ToNumber(int index)
{
	return lua_tonumber(L, index);
}

lua_Number LuaState::ToNumberX(int index, bool * isnum)
{
	return lua_tonumberx(L, index, reinterpret_cast<int*>(isnum));
}

const void * LuaState::ToPointer(int index)
{
	return lua_topointer(L, index);
}

const char* LuaState::ToString(int index)
{
	return lua_tostring(L, index);
}

lua_State * LuaState::ToThread(int index)
{
	return lua_tothread(L, index);
}

void * LuaState::ToUserData(int index)
{
	return lua_touserdata(L, index);
}

LuaTypes LuaState::Type(int index)
{
	return static_cast<LuaTypes>(lua_type(L, index));
}

const char * LuaState::Typename(LuaTypes tp)
{
	return lua_typename(L, static_cast<int>(tp));
}

int LuaState::UpvalueIndex(int index)
{
	return lua_upvalueindex(index);
}

const lua_Number * LuaState::Version()
{
	return lua_version(L);
}

void LuaState::XMove(lua_State * to, int n)
{
	return lua_xmove(L, to, n);
}

int LuaState::Yield(int nresults)
{
	return lua_yield(L, nresults);
}

int LuaState::YieldK(int nresults, lua_KContext ctx, lua_KFunction k)
{
	return lua_yieldk(L, nresults, ctx, k);
}

void LuaState::ArgCheck_L(int cond, int arg, const char * extramsg)
{
	return luaL_argcheck(L, cond, arg, extramsg);
}

int LuaState::ArgError_L(int arg, const char * extramsg)
{
	return luaL_argerror(L, arg, extramsg);
}

int LuaState::CallMeta_L(int obj, const char * e)
{
	return luaL_callmeta(L, obj, e);
}

void LuaState::CheckAny_L(int arg)
{
	return luaL_checkany(L, arg);
}

lua_Integer LuaState::CheckInteger_L(int arg)
{
	return luaL_checkinteger(L, arg);
}

const char * LuaState::CheckLstring_L(int arg, size_t * l)
{
	return luaL_checklstring(L, arg, l);
}

lua_Number LuaState::CheckNumber_L(int arg)
{
	return luaL_checknumber(L, arg);
}

int LuaState::CheckOption_L(int arg, const char * def, const char * const lst[])
{
	return luaL_checkoption(L, arg, def, lst);
}

void LuaState::CheckStack_L(int sz, const char * msg)
{
	return luaL_checkstack(L, sz, msg);
}

const char * LuaState::CheckString_L(int arg)
{
	return luaL_checkstring(L, arg);
}

void LuaState::CheckType_L(int arg, int t)
{
	return luaL_checktype(L, arg, t);
}

void * LuaState::CheckUData_L(int arg, const char * tname)
{
	return luaL_checkudata(L, arg, tname);
}

void LuaState::CheckVersion_L()
{
	return luaL_checkversion(L);
}

bool LuaState::DoFile_L(const char * filename)
{
	return luaL_dofile(L, filename);
}

bool LuaState::DoString_L(const char * str)
{
	return luaL_dostring(L, str) == false;
}

int LuaState::ExecResult_L(int stat)
{
	return luaL_execresult(L, stat);
}

int LuaState::FileResult_L(int stat, const char * fname)
{
	return luaL_fileresult(L, stat, fname);
}

int LuaState::GetMetaField_L(int obj, const char * e)
{
	return luaL_getmetafield(L, obj, e);
}

int LuaState::GetMetaTable_L(const char * tname)
{
	return luaL_getmetatable(L, tname);
}

int LuaState::GetSubTable_L(int idx, const char * fname)
{
	return luaL_getsubtable(L, idx, fname);
}

const char * LuaState::Gsub_L(const char * s, const char * p, const char * r)
{
	return luaL_gsub(L, s, p, r);
}

lua_Integer LuaState::Len_L(int index)
{
	return luaL_len(L, index);
}

LoadResults LuaState::LoadBuffer_L(const char * buff, size_t sz, const char * name)
{
	return static_cast<LoadResults>(luaL_loadbuffer(L, buff, sz, name));
}

LoadResults LuaState::LoadBufferX_L(const char * buff, size_t sz, const char * name, const char * mode)
{
	return static_cast<LoadResults>(luaL_loadbufferx(L, buff, sz, name, mode));
}

LoadResults LuaState::LoadFile_L(const char * filename)
{
	return static_cast<LoadResults>(luaL_loadfile(L, filename));
}

LoadResults LuaState::LoadFileX_L(const char * filename, const char * mode)
{
	return static_cast<LoadResults>(luaL_loadfilex(L, filename, mode));
}

LoadResults LuaState::LoadString_L(const char * s)
{
	return static_cast<LoadResults>((luaL_loadstring(L, s)));
}

void LuaState::NewLib_L(const luaL_Reg l[])
{
	return luaL_newlib(L, l);
}

void LuaState::NewLibTable_L(const luaL_Reg l[])
{
	return luaL_newlibtable(L, l);
}

int LuaState::NewMetaTable_L(const char * tname)
{
	return luaL_newmetatable(L, tname);
}

lua_State* LuaState::NewState_L(void)
{
	return luaL_newstate();
}

bool LuaState::OpenBit32()
{
	return luaopen_bit32(L) == 1;
}

bool LuaState::OpenBase()
{
	return luaopen_base(L) == 1;
}

bool LuaState::OpenCoroutine()
{
	return luaopen_coroutine(L) == 1;
}

bool LuaState::OpenDebug()
{
	return luaopen_debug(L) == 1;
}

bool LuaState::OpenIO()
{
	return luaopen_io(L) == 1;
}

bool LuaState::OpenMath()
{
	return luaopen_math(L) == 1;
}

bool LuaState::OpenOS()
{
	return luaopen_os(L) == 1;
}

bool LuaState::OpenPackage()
{
	return luaopen_package(L) == 1;
}

bool LuaState::OpenString()
{
	return luaopen_string(L) == 1;
}

bool LuaState::OpenTable()
{
	return luaopen_table(L) == 1;
}

bool LuaState::OpenUTF8()
{
	return luaopen_utf8(L) == 1;
}

void LuaState::OpenLibs_L()
{
	return luaL_openlibs(L);
}

lua_Integer LuaState::OptInteger_L(int arg, lua_Integer d)
{
	return luaL_optinteger(L, arg, d);
}

const char * LuaState::OptLString_L(int arg, const char * d, size_t * l)
{
	return luaL_optlstring(L, arg, d, l);
}

lua_Number LuaState::OptNumber_L(int arg, lua_Number d)
{
	return luaL_optnumber(L, arg, d);
}

const char * LuaState::OptString_L(int arg, const char * d)
{
	return luaL_optstring(L, arg, d);
}

int LuaState::Ref_L(int t)
{
	return luaL_ref(L, t);
}

void LuaState::RequireF_L(const char * modname, lua_CFunction openf, int glb)
{
	return luaL_requiref(L, modname, openf, glb);
}

void LuaState::SetFuncs_L(const luaL_Reg * l, int nup)
{
	return luaL_setfuncs(L, l, nup);
}

void LuaState::SetMetaTable_L(const char * tname)
{
	return luaL_setmetatable(L, tname);
}

void * LuaState::TestUData_L(int arg, const char * tname)
{
	return luaL_testudata(L, arg, tname);
}

const char * LuaState::ToLString_L(int idx, size_t * len)
{
	return luaL_tolstring(L, idx, len);
}

void LuaState::Traceback_L(lua_State * L1, const char * msg, int level)
{
	return luaL_traceback(L, L1, msg, level);
}

const char * LuaState::Typename_L(int index)
{
	return luaL_typename(L, index);
}

void LuaState::Unref_L(int t, int ref)
{
	return luaL_unref(L, t, ref);
}

void LuaState::Where_L(int lvl)
{
	return luaL_where(L, lvl);
}

LuaState::~LuaState()
{
	if (autoClose_)
	{
		assert(L);
		Close();
	}
}

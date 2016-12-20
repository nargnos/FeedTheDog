#pragma once
#include <utility>
#include <lua.hpp>

enum class LuaOps :int
{
	Add = LUA_OPADD,
	Sub = LUA_OPSUB,
	Mul = LUA_OPMUL,
	Mod = LUA_OPMOD,
	Pow = LUA_OPPOW,
	Div = LUA_OPDIV,
	FloorDiv = LUA_OPIDIV,
	BitAnd = LUA_OPBAND,
	BitOr = LUA_OPBOR,
	BitXor = LUA_OPBXOR,
	Shl = LUA_OPSHL,
	Shr = LUA_OPSHR,
	UnaryNegation = LUA_OPUNM,
	BitNot = LUA_OPBNOT,
};
enum class LuaCompareOps :int
{
	OpEQ = LUA_OPEQ,
	OpLT = LUA_OPLT,
	OpLE = LUA_OPLE,
};

enum class GcOps :int
{
	Stop = LUA_GCSTOP,
	Restart = LUA_GCRESTART,
	Collect = LUA_GCCOLLECT,
	Count = LUA_GCCOUNT,
	CountB = LUA_GCCOUNTB,
	Step = LUA_GCSTEP,
	SetPause = LUA_GCSETPAUSE,
	SetStepMul = LUA_GCSETSTEPMUL,
	IsRunning = LUA_GCISRUNNING,
};
enum class ThreadStatus :int
{
	Ok = LUA_OK,
	Yield = LUA_YIELD,
};
enum class CallResults :int
{
	Ok = LUA_OK,
	ErrRun = LUA_ERRRUN,
	ErrMem = LUA_ERRMEM,
	ErrGcMm = LUA_ERRGCMM,
	ErrErr = LUA_ERRERR,
};
enum class LoadResults :int
{
	Ok = LUA_OK,
	ErrSyntax = LUA_ERRSYNTAX,
	ErrMem = LUA_ERRMEM,
	ErrGcMm = LUA_ERRGCMM,
};
enum class LuaTypes
{
	TNone = LUA_TNONE,
	TNil = LUA_TNIL,
	TBoolean = LUA_TBOOLEAN,
	TLightUserdata = LUA_TLIGHTUSERDATA,
	TNumber = LUA_TNUMBER,
	TString = LUA_TSTRING,
	TTable = LUA_TTABLE,
	TFunction = LUA_TFUNCTION,
	TUserdata = LUA_TUSERDATA,
	TThread = LUA_TTHREAD,
	NUMTAGS = LUA_NUMTAGS,
};
class LuaState
{
public:
	LuaState();
	LuaState(lua_Alloc f, void * ud);
	LuaState(lua_State* state, bool autoClose = false);
	~LuaState();
	lua_State* RawPtr() const;
	template<typename... TArgs>
	const char* PushFString(const char* fmt, TArgs&&... args)
	{
		return lua_pushfstring(L, _STD forward<TArgs>(args)...);
	}
	template<typename... TArgs>
	int Error_L(const char *fmt, TArgs&&... args)
	{
		return luaL_error(L, fmt, _STD forward<TArgs>(args)...);
	}
	template<typename TFunc, typename TDefault>
	static auto Opt_L(lua_State*L, TFunc&& func, int argIdx, TDefault&& dflt)
	{
		return luaL_opt(L, func, argIdx, dflt);
	}

	int AbsIndex(int index);
	void Arith(LuaOps op);
	lua_CFunction AtPanic(lua_CFunction panicf);
	void Call(int nargs, int nresults);
	void Callk(int nargs, int nresults, lua_KContext ctx, lua_KFunction k);
	int CheckStack(int n);
	void Close();
	int Compare(int left, int right, LuaCompareOps op);
	void Concat(int n);
	void Copy(int fromIdx, int toIdx);
	void CreateTable(int narr, int nrec);
	int Dump(lua_Writer writer, void* data, int strip);
	int Error();
	int Gc(GcOps what, int data);
	lua_Alloc GetAllocF(void **ud);
	void* GetExtraSpace();
	int GetField(int index, const char *key);
	int GetGlobal(const char* name);
	int GetI(int index, lua_Integer i);
	int GetMetaTable(int index);
	int GetTable(int index);
	int GetTop();
	int GetUserValue(int index);
	void Insert(int index);
	bool IsBoolean(int index);
	bool IsCFunction(int index);
	bool IsFunction(int index);
	bool IsInteger(int index);
	bool IsLightUserData(int index);
	bool IsNil(int index);
	bool IsNone(int index);
	bool IsNoneOrNil(int index);
	bool IsNumber(int index);
	bool IsString(int index);
	bool IsTable(int index);
	bool IsThread(int index);
	bool IsUserData(int index);
	bool IsYieldable();
	void Len(int index);
	LoadResults Load(lua_Reader reader, void *data, const char *chunkname, const char *mode);
	void NewTable();
	lua_State* NewThread();
	void* NewUserData(size_t size);
	int Next(int index);
	CallResults PCall(int nargs, int nresults, int msgh);
	CallResults PCallK(int nargs, int nresults, int msgh, lua_KContext ctx, lua_KFunction k);
	void Pop(int n);
	void PushBoolean(bool val);
	void PushCClosure(lua_CFunction fn, int n);
	void PushCFunction(lua_CFunction f);
	void PushGlobalTable();
	void PushInteger(lua_Integer val);
	void PushLightUserData(void *p);
	const char* PushLString(const char *s, size_t len);
	void PushNil();
	void PushNumber(lua_Number n);
	const char* PushString(const char *s);
	int PushThread();
	void PushValue(int index);
	const char* PushVFString(const char *fmt, va_list argp);
	int RawEqual(int left, int right);
	int RawGet(int index);
	int RawGetI(int index, lua_Integer n);
	int RawGetP(int index, const void *p);
	size_t RawLen(int index);
	void RawSet(int index);
	void RawSetI(int index, lua_Integer i);
	void RawSetP(int index, const void *p);
	void Register(const char *name, lua_CFunction f);
	void Remove(int index);
	void Replace(int index);
	int Resume(lua_State *from, int nargs);
	void Rotate(int idx, int n);
	void SetAllocF(lua_Alloc f, void *ud);
	void SetField(int index, const char *k);
	void SetGlobal(const char *name);
	void SetI(int index, lua_Integer n);
	int SetMetaTable(int index);
	void SetTable(int index);
	void SetTop(int index);
	void SetUserValue(int index);
	ThreadStatus Status();
	size_t StringToNumber(const char* s);
	bool ToBoolean(int index);
	lua_CFunction ToCFunction(int index);
	lua_Integer ToInteger(int index);
	lua_Integer ToIntegerx(int index, bool* isnum);
	const char* ToLString(int index, size_t *len);
	lua_Number ToNumber(int index);
	lua_Number ToNumberX(int index, bool* isnum);
	const void * ToPointer(int index);
	const char* ToString(int index);
	lua_State * ToThread(int index);
	void * ToUserData(int index);
	LuaTypes Type(int index);
	const char * Typename(LuaTypes tp);
	const lua_Number * Version();
	void XMove(lua_State *to, int n);
	int Yield(int nresults);
	int YieldK(int nresults, lua_KContext ctx, lua_KFunction k);
	static lua_State* NewState(lua_Alloc f, void *ud);
	static int NumberToInteger(lua_Number n, lua_Integer *p);
	static int UpvalueIndex(int index);

	void ArgCheck_L(int cond, int arg, const char *extramsg);
	int ArgError_L(int arg, const char *extramsg);
	int CallMeta_L(int obj, const char *e);
	void CheckAny_L(int arg);
	lua_Integer CheckInteger_L(int arg);
	const char * CheckLstring_L(int arg, size_t *l);
	lua_Number CheckNumber_L(int arg);
	int CheckOption_L(int arg, const char *def, const char *const lst[]);
	void CheckStack_L(int sz, const char *msg);
	const char * CheckString_L(int arg);
	void CheckType_L(int arg, int t);
	void * CheckUData_L(int arg, const char *tname);
	void CheckVersion_L();
	bool DoFile_L(const char *filename);
	bool DoString_L(const char *str);
	int ExecResult_L(int stat);
	int FileResult_L(int stat, const char *fname);
	int GetMetaField_L(int obj, const char *e);
	int GetMetaTable_L(const char *tname);
	int GetSubTable_L(int idx, const char *fname);
	const char * Gsub_L(const char *s, const char *p, const char *r);
	lua_Integer Len_L(int index);
	LoadResults LoadBuffer_L(const char *buff, size_t sz, const char *name);
	LoadResults LoadBufferX_L(const char *buff, size_t sz, const char *name, const char *mode);
	LoadResults LoadFile_L(const char *filename);
	LoadResults LoadFileX_L(const char *filename, const char *mode);
	LoadResults LoadString_L(const char *s);
	void NewLib_L(const luaL_Reg l[]);
	void NewLibTable_L(const luaL_Reg l[]);
	int NewMetaTable_L(const char *tname);
	void OpenLibs_L();
	lua_Integer OptInteger_L(int arg, lua_Integer d);
	const char * OptLString_L(int arg, const char *d, size_t *l);
	lua_Number OptNumber_L(int arg, lua_Number d);
	const char * OptString_L(int arg, const char *d);
	int Ref_L(int t);
	void RequireF_L(const char *modname, lua_CFunction openf, int glb);
	void SetFuncs_L(const luaL_Reg *l, int nup);
	void SetMetaTable_L(const char *tname);
	void * TestUData_L(int arg, const char *tname);
	const char * ToLString_L(int idx, size_t *len);
	void Traceback_L(lua_State *L1, const char *msg, int level);
	const char * Typename_L(int index);
	void Unref_L(int t, int ref);
	void Where_L(int lvl);
	static lua_State* NewState_L(void);

	bool OpenBit32();
	bool OpenBase();
	bool OpenCoroutine();
	bool OpenDebug();
	bool OpenIO();
	bool OpenMath();
	bool OpenOS();
	bool OpenPackage();
	bool OpenString();
	bool OpenTable();
	bool OpenUTF8();

private:
	LuaState(const LuaState&) = delete;
	LuaState& operator=(const LuaState&) = delete;
	lua_State* L;
	bool autoClose_;
};

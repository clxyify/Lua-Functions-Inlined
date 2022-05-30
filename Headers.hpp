//Hey! Don't go anywhere! This file actually requires global updating

#define ttisnumber(o)	(ttype(o) == LUA_TNUMBER)//This updates often, it's called R_LUA_TNUMBER
#define check_exp(c,e)		(e)//No need for update
#define ttype(o)	((o)->tt)//No need for update
#define tonumber(o,n)	(ttype(o) == 3 || \//This updates often, it's called R_LUA_TNUMBER
                         (((o) = luaV_tonumber(o,n)) != 0))
				 
//No need to update anything here, it's just headers from Lua source				 
struct r_GCheader
{
	BYTE tt;
	BYTE marked;
	BYTE _pad;
};

union r_GCObject
{
	r_GCheader gch;
	std::uintptr_t ts; /* tstring */
	std::uintptr_t u; /* userdata */
	std::uintptr_t cl; /* closure */
	std::uintptr_t h; /* table */
	std::uintptr_t p; /* pointer */
	std::uintptr_t uv; /* upvalue */
	std::uintptr_t th;  /* thread */
};

typedef union
{
	r_GCObject* gc;
	void* p;
	std::double_t n;
	int b;
} r_Value;

typedef struct r_lua_TValue
{
	r_Value value;
	r_lua_TValue* ptr;
	int tt;
} r_TValue;

typedef r_TValue* r_StkId;

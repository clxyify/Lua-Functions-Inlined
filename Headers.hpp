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

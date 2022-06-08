//Hey! Don't go anywhere! This file actually requires global updating

#define ttisnumber(o)	(ttype(o) == LUA_TNUMBER)//This updates often, it's called R_LUA_TNUMBER
#define check_exp(c,e)		(e)//No need for update
#define ttype(o)	((o)->tt)//No need for update
#define UNUSED(x)	((void)(x))	/* to avoid warnings */
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

#define LUAI_GCGOAL 200    /* 200% (allow heap to double compared to live heap size) */
#define LUAI_GCSTEPMUL 200 /* GC runs 'twice the speed' of memory allocation */
#define LUAI_GCSTEPSIZE 1  /* GC runs every KB of memory allocation */

/*
** Possible states of the Garbage Collector
*/
#define GCSpause 0
#define GCSpropagate 1
#define GCSpropagateagain 2
#define GCSatomic 3
#define GCSsweep 4

/*
** macro to tell when main invariant (white objects cannot point to black
** ones) must be kept. During a collection, the sweep
** phase may break the invariant, as objects turned white may point to
** still-black objects. The invariant is restored when sweep ends and
** all objects are white again.
*/
#define keepinvariant(g) ((g)->gcstate == GCSpropagate || (g)->gcstate == GCSpropagateagain || (g)->gcstate == GCSatomic)

/*
** some useful bit tricks
*/
#define resetbits(x, m) ((x) &= cast_to(uint8_t, ~(m)))
#define setbits(x, m) ((x) |= (m))
#define testbits(x, m) ((x) & (m))
#define bitmask(b) (1 << (b))
#define bit2mask(b1, b2) (bitmask(b1) | bitmask(b2))
#define l_setbit(x, b) setbits(x, bitmask(b))
#define resetbit(x, b) resetbits(x, bitmask(b))
#define testbit(x, b) testbits(x, bitmask(b))
#define set2bits(x, b1, b2) setbits(x, (bit2mask(b1, b2)))
#define reset2bits(x, b1, b2) resetbits(x, (bit2mask(b1, b2)))
#define test2bits(x, b1, b2) testbits(x, (bit2mask(b1, b2)))

/*
** Layout for bit use in `marked' field:
** bit 0 - object is white (type 0)
** bit 1 - object is white (type 1)
** bit 2 - object is black
** bit 3 - object is fixed (should not be collected)
*/

#define WHITE0BIT 0
#define WHITE1BIT 1
#define BLACKBIT 2
#define FIXEDBIT 3
#define WHITEBITS bit2mask(WHITE0BIT, WHITE1BIT)

#define iswhite(x) test2bits((x)->gch.marked, WHITE0BIT, WHITE1BIT)
#define isblack(x) testbit((x)->gch.marked, BLACKBIT)
#define isgray(x) (!testbits((x)->gch.marked, WHITEBITS | bitmask(BLACKBIT)))
#define isfixed(x) testbit((x)->gch.marked, FIXEDBIT)

#define otherwhite(g) (g->currentwhite ^ WHITEBITS)
#define isdead(g, v) (((v)->gch.marked & (WHITEBITS | bitmask(FIXEDBIT))) == (otherwhite(g) & WHITEBITS))

#define changewhite(x) ((x)->gch.marked ^= WHITEBITS)
#define gray2black(x) l_setbit((x)->gch.marked, BLACKBIT)

#define luaC_white(g) cast_to(uint8_t, ((g)->currentwhite) & WHITEBITS)

// Thread stack states
#define THREAD_ACTIVEBIT 0   // thread is currently active
#define THREAD_SLEEPINGBIT 1 // thread is not executing and stack should not be modified

#define luaC_threadactive(L) (testbit((L)->stackstate, THREAD_ACTIVEBIT))
#define luaC_threadsleeping(L) (testbit((L)->stackstate, THREAD_SLEEPINGBIT))

#define luaC_checkGC(L) \
    { \
        condhardstacktests(luaD_reallocstack(L, L->stacksize - EXTRA_STACK)); \
        if (L->global->totalbytes >= L->global->GCthreshold) \
        { \
            condhardmemtests(luaC_validate(L), 1); \
            luaC_step(L, true); \
        } \
        else \
        { \
            condhardmemtests(luaC_validate(L), 2); \
        } \
    }

#define luaC_barrier(L, p, v) \
    { \
        if (iscollectable(v) && isblack(obj2gco(p)) && iswhite(gcvalue(v))) \
            luaC_barrierf(L, obj2gco(p), gcvalue(v)); \
    }

#define luaC_barriert(L, t, v) \
    { \
        if (iscollectable(v) && isblack(obj2gco(t)) && iswhite(gcvalue(v))) \
            luaC_barriertable(L, t, gcvalue(v)); \
    }

#define luaC_barrierfast(L, t) \
    { \
        if (isblack(obj2gco(t))) \
            luaC_barrierback(L, t); \
    }

#define luaC_objbarrier(L, p, o) \
    { \
        if (isblack(obj2gco(p)) && iswhite(obj2gco(o))) \
            luaC_barrierf(L, obj2gco(p), obj2gco(o)); \
    }

#define luaC_upvalbarrier(L, uv, tv) \
    { \
        if (iscollectable(tv) && iswhite(gcvalue(tv)) && (!(uv) || (uv)->v != &(uv)->u.value)) \
            luaC_barrierupval(L, gcvalue(tv)); \
    }

#define luaC_checkthreadsleep(L) \
    { \
        if (luaC_threadsleeping(L)) \
            luaC_wakethread(L); \
    }

#define luaC_init(L, o, tt) luaC_initobj(L, cast_to(GCObject*, (o)), tt)

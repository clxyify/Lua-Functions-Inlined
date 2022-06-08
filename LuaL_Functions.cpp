//luaL is from the aux api to give better functions so yeah.....
//kinda useless cuz we don't know what can we use this for.

void r_luaL_sandboxthread(DWORD rL)//sandboxthread
{
    // create new global table that proxies reads to original table
    r_lua_newtable(rL);

    r_lua_newtable(rL);
    r_lua_pushvalue(rL, LUA_GLOBALSINDEX);
    r_lua_setfield(rL, -2, "__index");
    r_lua_setreadonly(rL, -1, true);

    r_lua_setmetatable(rL, -2);//set it to -2

    // we can set safeenv now although it's important to set it to false if code is loaded twice into the thread
    r_lua_replace(rL, LUA_GLOBALSINDEX);
    r_lua_setsafeenv(rL, LUA_GLOBALSINDEX, 1); //also known as bool true;
}

void r_luaL_sandbox(lua_State* L)//no thread
{
    // set all libraries to read-only
    r_lua_pushnil(rL);
    while (r_lua_next(rL, LUA_GLOBALSINDEX) != NULL)//NULL
    {
        if (r_lua_istable(rL, -1))
            r_lua_setreadonly(rL, -1, true);

        r_lua_pop(rL, 1);
    }

    // set all builtin metatables to read-only
    r_lua_pushliteral(rL, "");
    r_lua_getmetatable(rL, -1);
    r_lua_setreadonly(rL, -1, 1);//make true
    r_lua_pop(rL, 2);//pop

    // set globals to readonly and activate safeenv since the env is immutable
    r_lua_setreadonly(rL, LUA_GLOBALSINDEX, true);//set it to true
    r_lua_setsafeenv(rL, LUA_GLOBALSINDEX, true);//set it to true
}

const char* r_luaL_findtable(DWORD rL, int idx, const char* fname, int szhint)
{
    const char* e;
    r_lua_pushvalue(rL, idx);
    do
    {
        e = strchr(fname, '.');
        if (e == NULL)
            e = fname + strlen(fname);
        r_lua_pushlstring(rL, fname, e - fname);//did you know all pushstring does is check if string is empty and push nil? ikr
        r_lua_rawget(rL, -2);
        if (r_lua_isnil(rL, -1))
        {                                                    /* no such field? */
            r_lua_pop(rL, 1);                                   /* remove this nil */
            r_lua_createtable(rL, 0, (*e == '.' ? 1 : szhint)); /* new table for field */
           r_lua_pushlstring(rL, fname, e - fname);
            r_lua_pushvalue(rL, -2);
            r_lua_settable(rL, -4); /* set new table into field */
        }
        else if (!r_lua_istable(rL, -1))
        {                  /* field has a non-table value? */
            r_lua_pop(rL, 2); /* remove table and value */
            return fname;  /* return problematic part of the name */
        }
        r_lua_remove(rL, -2); /* remove previous table */
        fname = e + 1;
    } while (*e == '.');
    return NULL;
}

const char* luaL_typename(DWORD rL, int idx)//type name but L
{
    const TValue* obj = r_luaA_toobject(rL, idx);
    return r_luaT_objtypename(rL, obj);
}

const char* r_luaL_tolstring(DWORD rL, int idx, size_t* len)
{
    if (r_luaL_callmeta(rL, idx, "__tostring")) /* is there a metafield? */
    {
        if (!r_lua_isstring(rL, -1))//if no string is returned
            luaL_error(L, "'__tostring' must return a string");
            /* return as error tostring must return string */
        return lua_tolstring(L, -1, len);
    }

    switch (r_lua_type(rL, idx))//all globals below get updated btw^^
    {
    case LUA_TNUMBER:
    {
        double n = r_lua_tonumber(rL, idx);
        char s[48];
        char* e = r_luai_num2str(s, n);
        r_lua_pushlstring(rL, s, e - s);
        break;
    }
    case LUA_TSTRING:
        r_lua_pushvalue(rL, idx);
        break;
    case LUA_TBOOLEAN:
        r_lua_pushstring(rL, (r_lua_toboolean(rL, idx) ? "true" : "false"));
        break;
    case LUA_TNIL:
        r_lua_pushliteral(rL, "nil");
        break;
    case LUA_TVECTOR:
    {
        const float* v = r_lua_tovector(rL, idx);

        char s[48 * LUA_VECTOR_SIZE];//diy you find it yourself!
        char* e = s;
        for (int i = 0; i < LUA_VECTOR_SIZE; ++i)
        {
            if (i != 0)
            {
                *e++ = ',';
                *e++ = ' ';
            }
            e = r_luai_num2str(e, v[i]);
        }
        r_lua_pushlstring(rL, s, e - s);
        break;
    }
    default:
    {
        const void* ptr = r_lua_topointer(rL, idx);
        unsigned long long enc = r_lua_encodepointer(rL, uintptr_t(ptr));
        r_lua_pushfstring(rL, "%s: 0x%016llx", r_luaL_typename(rL, idx), enc);
        //use pushlstring or any other function related to push-string
        break;
    }
    }
    return r_lua_tolstring(rL, -1, len);
}

void r_luaL_checkstack(DWORD rL, int space, const char* mes)//aux check stack
{
    mes = mes;//message
    if (!r_lua_checkstack(rL, space))
        r_luaL_error(rL, "stack overflow (%s)", mes);//error
    else
        //no stack overflow :)
}

int r_luaL_checkoption(DWORD rL, int narg, const char* def, const char* const lst[])//4 args
{
    const char* name = (def) ? r_luaL_optstring(rL, narg, def) : r_luaL_checkstring(rL, narg);
    int i;
    for (i = 0; lst[i]; i++)
        if (strcmp(lst[i], name) == 0)
            return i;
    const char* msg = r_lua_pushfstring(rL, "invalid option '%s'", name);//not a option and push to argument error
    r_luaL_argerrorL(rL, narg, msg);//arg error 'invalid option' internal argument
}

int r_luaL_newmetatable(DWORD rL, const char* tname)//creates new metatable with 2nd argument tname
{
    int value = -1;
    r_lua_getfield(rL, LUA_REGISTRYINDEX, tname); /* get registry.name */
    if (!r_lua_isnil(rL, value))                     /* name already in use? */
        return NULL;                              /* leave previous value on top, but return 0 */
    r_lua_pop(rL, 1);//note: this is settop used as define to make life 100% easier
    r_lua_newtable(rL); /* create metatable */
    r_lua_pushvalue(rL, value);
    r_lua_setfield(rL, LUA_REGISTRYINDEX, tname); /* registry.name = metatable */
    return 1;
}

void* r_luaL_checkudata(DWORD rL, int ud, const char* tname)//check user data aux
{
    void* p = r_lua_touserdata(rL, ud);//p* now to userdata
    if (p != 0)//equal to 0
    { /* value is a userdata? */
        if (r_lua_getmetatable(rL, ud))
        {                                              /* does it have a metatable? */
            r_lua_getfield(rL, LUA_REGISTRYINDEX, tname); /* get correct metatable */
            if (r_lua_rawequal(rL, -1, -2))
            {                  /* does it have the correct mt? */
                r_lua_pop(L, 2); /* remove both metatables settop */
                return p;
            }
        }
    }
    std::cout << "error while function was starting";//error comes :SKULL:
    //i suggest you make ur exploit console based temporary
}

void r_luaL_checkstack(DWORD rL, int space, const char* mes)//checks stack 3 args
{
    if (!r_lua_checkstack(rL, space))//! = not
        r_luaL_error(rL, "stack overflow (%s)", mes);
        std::cout << "encountered overflow " << mes;
    else//no overflow but should loop this func to check overflowed stack
        std::cout << "no overflow (YET)";
}

void r_luaL_checktype(DWORD rL, int narg, int t)//checks type with 3 arg a integer
{
    if (r_lua_type(rL, narg) != t)
        std::cout << narg;//display in console
    else
        //not
}

void r_luaL_checkany(DWORD rL, int narg)//checks narg
{
    if (r_lua_type(rL, narg) == R_LUA_TNONE)//global offset
        r_luaL_error(L, "missing argument #%d", narg);
    std::cout << "did u miss an argument?";
}

const char* r_luaL_checklstring(DWORD rL, int narg, size_t* len)//checks len for string
{
    const char* s = r_lua_tolstring(rL, narg, len);
    if (!s)
        tag_error(L, narg, LUA_TSTRING);//errror
    return s;
}

const char* r_luaL_optlstring(DWORD rL, int narg, const char* def, size_t* len)//optlstring function 3 arg
{
    if (r_lua_isnoneornil(rL, narg))
    {
        if (len)
            *len = (def ? strlen(def) : NULL);
        return def;
    }
    else
        return r_luaL_checklstring(rL, narg, len);
}

double luaL_checknumber(DWORD rL, int narg)//why not make it a double?
{
    int isnum;
    double d = r_lua_tonumberx(rL, narg, &isnum);
    if (!isnum)
        tag_error(L, narg, LUA_TNUMBER);//global offset
    return d;
}

double luaL_optnumber(DWORD rL, int narg, double def)//opt number with L check number
{
    return r_luaL_opt(rL, r_luaL_checknumber, narg, def);
}

static void* l_alloc(void* ud, void* ptr, size_t osize, size_t nsize)//need this for lua_state btw
{
    (void)ud;
    (void)osize;
    if (nsize == 0)
    {
        free(ptr);
        return NULL;
    }
    else
        return realloc(ptr, nsize);
}

DWORD* luaL_newstate(void)//fake ofc and not even inlined lmao
{
    return lua_newstate(l_alloc, NULL);
}

int r_luaL_callmeta(DWORD rL, int obj, const char* event)//meta caller
{
    obj = abs_index(rL, obj); //alternative way down below uncomment it
    //(obj) > NULL || (i) <= LUA_REGISTRYINDEX ? (i) : r_lua_gettop(rL) + (obj) + 1);
    if (!r_luaL_getmetafield(rL, obj, event)) /* no metafield? */
        return 0;
    r_lua_pushvalue(rL, obj);//push int 'obj' aka abs_index
    r_lua_call(rL, 1, 1);
    //r_lua_pcall(rL, 1, 1, 0);
    return 1;
}

int r_luaL_checkoption(DWORD rL, int narg, const char* def, const char* const lst[])//checks option or returns 'invalid'
{
    const char* name = (def) ? r_luaL_optstring(rL, narg, def) : r_luaL_checkstring(rL, narg);
    int i;
    for (i = 0; lst[i]; i++)
        if (strcmp(lst[i], name) == 0)//checks if i is NULL
            return i;
    const char* msg = r_lua_pushfstring(rL, "invalid option '%s'", name);//return error
    r_luaL_argerrorL(rL, narg, msg);
}

int luaL_checkboolean(DWORD rL, bool narg)//use bool instead of int
{
    #define RLUA_TBOOLEAN 15//tboolean offset (global)
    if (!r_lua_isboolean(rL, narg))//hot
        luaL_typeerrorL(rL, narg, lua_typename(rL, RLUA_TBOOLEAN));
    return r_lua_toboolean(rL, narg);
}

int luaL_optboolean(DWORD rL, int narg, int def)
{
    return r_luaL_opt(rL, r_luaL_checkboolean, narg, def);
}

int r_luaL_checkinteger(DWORD rL, int narg)//check integer
{
    int isnum;
    int d = r_lua_tointegerx(rL, narg, &isnum);
    if (!isnum)//not number
        r_luaL_error(rL, "not a valid number");//custom error
    return d;
}

int r_luaL_optinteger(DWORD rL, int narg, int def)//opts integer
{
    return r_luaL_opt(rL, r_luaL_checkinteger, narg, def);
}

unsigned luaL_checkunsigned(DWORD rL, int narg)
{
    int isnum;
    unsigned d = r_lua_tounsignedx(rL, narg, &isnum);
    if (!isnum)
        luaL_typeerrorL(rL, narg, lua_typename(rL, RLUA_TONUMBER));//use that
    return d;
}

unsigned luaL_optunsigned(DWORD rL, int narg, unsigned def)
{
    return r_luaL_opt(rL, r_luaL_checkunsigned, narg, def);
}

//more to come soon :)

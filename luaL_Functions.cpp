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

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

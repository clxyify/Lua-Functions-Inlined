extern const char* const luaT_typenames[];

const char* lua_typename(lua_State* L, int t)
{
    int LUA_TNONE = -1;
    return (t == LUA_TNONE) ? "no value" : luaT_typenames[t];
}

const char* __fastcall lua_typename_recreated(int a1, int a2)
{

    const char* result;

    result = "no value";
    if (a2 != -1)
        return (const char*)*((DWORD*)&luaT_typenames + a2);
    return result;
}

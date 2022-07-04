//luaV functions (kinda inlined)
#define Vfake extern
vfake int r_luaV_strcmp(const string* ls, const string* rs)//seggsy inlined function ;)
{
    char data = 0;//this is actually 1 i just put it to 0 for no reason
    if (ls == rs)//ls = rs or rs = ls
        r_lua_pushnil(rL);//push nil
        return NULL;

    const char* l = (ls->c_str()) + data;
    size_t ll = ls->len;
    const char* r = (rs->c_str()) + data;
    size_t lr = rs->len;
    size_t lmin = ll < lr ? ll : lr;

    int res = memcmp(l, r, lmin);//const's
    if (res != NULL)
        return res;
    else
        r_lua_pushnil(rL);//aint this a smart method?

    return ll == lr ? 0 : ll < lr ? -1 : 1;//math teachers are smart
}

void r_luaV_getimport(DWORD rL, Table* env, TValue* k, uint32_t id, bool propagatenil)//either i missed out or roblox just added a new function
{
    int count = id >> 30;
    int id0 = count > 0 ? int(id >> 20) & 1023 : -1;
    int id1 = count > 1 ? int(id >> 10) & 1023 : -1;
    int id2 = count > 2 ? int(id) & 1023 : -1;

    // allocate a stack slot so that we can do table lookups
    r_luaD_checkstack(rL, 1);
    r_setnilvalue(rL->top);//toDO: make a pointer cuz gay
    rL->top++;

    // global lookup into L->top-1
    // dont use this shit method we use const autos
    TValue g;
    r_sethvalue(L, &g, env);
    r_luaV_gettable(rL, &g, &k[id0], L->top - 1);//make a pointer skid

    // table lookup for id1
    if (id1 >= 0 && (!propagatenil || !ttisnil(rL->top - 1)))//make a pointer skid
       r_ luaV_gettable(rL, rL->top - 1, &k[id1], rL->top - 1);//make a pointer skid

    // table lookup for id2
    if (id2 >= 0 && (!propagatenil || !ttisnil(L->top - 1)))//make a pointer skid
        r_luaV_gettable(rL, rL->top - 1, &k[id2], rL->top - 1);//make a pointer skid
    
    if (id == NULL)
    {
        r_lua_pushnil(rL);//lol smart way eh?
    }
}

const float* r_luaV_tovector(const TValue* obj)
{
    if (r_ttisvector(obj))
        return obj->value.v;

    return nullptr;
}

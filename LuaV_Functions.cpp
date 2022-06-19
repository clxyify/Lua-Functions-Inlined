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

//luaA functions

const TValue* r_luaA_toobject(DWORD rL, int idx)
{
    StkId p = index2addr(rL, idx);
    return (p == r_luaO_nilobject) ? 0 : p;
}

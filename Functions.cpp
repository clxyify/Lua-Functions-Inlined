extern double rlua_tonumber (DWORD rL, int idx)//Converts the Lua value at the given index to the C type lua_Number
{
  TValue n;//Included in "Headers.hpp"
  const TValue *o = r_lua_index2adr(rL, idx);//Index2adr function required
  if (tonumber(o, &n))
  {
    return nvalue(o);
  }
  else
  {
      return 0;
  }
}

extern int rlua_toboolean (DWORD rL, int idx) {//Converts the Lua value at the given index to a C boolean value
  const TValue *o = r_lua_index2adr(rL, idx);
  return !l_isfalse(o);//If errors, go to lobject.h and copy the refrences
}

extern int rlua_type (DWORD rL, int idx) {//Returns the type of the value in the given valid index
  StkId o = r_lua_index2adr(rL, idx);//External type, not a lua API function
  return (o == luaO_nilobject) ? -1 : ttype(o);//Requires some lua files
}

extern const char *lua_typename (DWORD rL, int t) {
  return (t == LUA_TNONE) ? "no value" : luaT_typenames[t];
}

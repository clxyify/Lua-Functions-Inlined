//So this file contains some inlined and working functions! woo-hoo!
//I made most of these, and some found so ill just cred them If i took them!
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
      return NULL;
  }
}

extern int rlua_toboolean (DWORD rL, int idx) {//Converts the Lua value at the given index to a C boolean value
  const TValue *o = r_lua_index2adr(rL, idx);
  return !l_isfalse(o);//If errors, go to lobject.h and copy the refrences
}

extern int rlua_type (DWORD rL, int idx) {//Returns the type of the value in the given valid index
  StkId o = r_lua_index2adr(rL, idx);//External type, not a lua API function
  return (o == luaO_nilobject) ? -1 : ttype(o);//-1 is R_LUA_TNONE which requires updating
}

extern const char *rlua_typename (DWORD rL, int t) {
  UNUSED(rL);//You can comment this out if erroring ;)
  return (t == -1) ? "no value douchebag" : luaT_typenames[t];//-1 is R_LUA_TNONE which requires updating
}

extern const void *rlua_topointer (DWORD rL, int idx) {//Points lua at Lua where it points Lua at Lua where it Points Lua at Lua where it Points Lua at Lua
  StkId o = r_lua_index2adr(rL, idx);
  switch (ttype(o)) {
    case LUA_TTABLE: return hvalue(o);
    case LUA_TFUNCTION: return clvalue(o);
    case LUA_TTHREAD: return thvalue(o);
    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA:
      return r_lua_touserdata(rL, idx);//This function is required as it is a external type
    default: return 0;
  }
}

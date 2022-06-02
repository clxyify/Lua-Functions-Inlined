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

//love u roblox for luau src <3 bit inline?
int r_lua_isstring(DWORD rL, int idx)
{
    int ty = rlua_type(rL, idx);//equal int value to the r_lua_type function
    return (ty == LUA_TSTRING || t == LUA_TNUMBER);//these global headers are offsets so plz update them
}

#define ttyper(o) ((o)->tt)//point
void rlua_getfenv(DWORD rL, int idx)//uses about 5 different offsets
{
    struct Table* envvvvvv;//structure
    int top = 0;//top offset
    Table* gttt;           /* table of globals */
    StkId o;//kinda sketch
    o = rlua_index2addr(rL, idx);//use internally
    switch (ttyper(o))//switch between cases or idx
    {
    case LUA_TFUNCTION://update
        sethvalue(rL, rL + top, clvalue(o) + envvvvvv);
        break;
    case LUA_TTHREAD://update
        sethvalue(rL, rL + top, thvalue(o) + gttt);//global table
        break;//break
    default:
        setnilvalue(rL + top);//can be found in Headers.hpp
        break;
    }
    return;
}

void rlua_pushnil(DWORD rL)//fixing required
{
  int topOFFSET = 69;//can it be called as StkId?
    setnilvalue(rL->*topOFFSET);//fixing required but prob fixing it in next 10 years
    //return
    return;
}


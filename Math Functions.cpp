//math library functions if you dont know what it is
//roblox technically becomes a nerd and does math so here ya go
//it should work and u can modify it and skid it so idrc but here you go
static float fade(float t)//bye
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

static float lerp(float t, float a, float b)//leerp
{
    return a + t * (b - a);
}

static float grad(unsigned char hash, float x, float y, float z)//grad vector3?
{
    unsigned char h = hash & 15;
    float u = (h < 8) ? x : y;
    float v = (h < 4) ? y : (h == 12 || h == 14) ? x : z;

    return (h & 1 ? -u : u) + (h & 2 ? -v : v);
}

static int math_noise(DWORD rL)//math noise
{
    //for the stupids out their, optnumber = lual_optinteger
    double x = r_luaL_checknumber(rL, 1);
    double y = r_luaL_optnumber(rL, 2, 0.0);
    double z = r_luaL_optnumber(rL, 3, 0.0);

    double r = perlin((float)x, (float)y, (float)z);

    r_lua_pushnumber(rL, r);//push floats x y z

    return 1;
}

static int math_clamp(DWORD rL)//clamp
{
    double v = r_luaL_checknumber(rL, 1);//DWORD a1, int a2
    double min = r_luaL_checknumber(rL, 2);
    double max = r_luaL_checknumber(rL, 3);//check numbedr

    luaL_argcheck(L, min <= max, 3, "max must be greater than or equal to min");//i hate this

    double r = v < min ? min : v;
    r = r > max ? max : r;

    r_lua_pushnumber(rL, r);//push operanders
    return 1;
}

static int math_sign(DWORD rL)//sign
{
    double v = r_luaL_checknumber(rL, 1);//todo: i hate v
    r_lua_pushnumber(rL, v > 0.0 ? 1.0 : v < 0.0 ? -1.0 : 0.0);//time to get ur brain fucked :D
    return 1;
}

static int math_max(lua_State* L)
{
    int n = lua_gettop(L); /* number of arguments */
    double dmax = luaL_checknumber(L, 1);
    int i;
    for (i = 2; i <= n; i++)
    {
        double d = luaL_checknumber(L, i);
        if (d > dmax)
            dmax = d;
    }
    lua_pushnumber(L, dmax);
    return 1;
}

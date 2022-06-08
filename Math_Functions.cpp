//math library functions if you dont know what it is
//roblox technically becomes a nerd and does math so here ya go
//it should work and u can modify it and skid it so idrc but here you go
//btw to register use your custom register env system and anyways enjoy!

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

static int math_abs(DWORD rL)
{
    r_lua_pushnumber(rL, fabs(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_sin(DWORD rL)
{
    r_lua_pushnumber(rL, sin(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_sinh(DWORD rL)
{
    r_lua_pushnumber(rL, sinh(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_cos(DWORD rL)
{
    r_lua_pushnumber(rL, cos(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_cosh(DWORD rL)
{
    r_lua_pushnumber(rL, cosh(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_tan(DWORD rL)
{
    r_lua_pushnumber(rL, tan(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_tanh(DWORD rL)
{
    r_lua_pushnumber(rL, tanh(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_asin(DWORD rL)
{
    r_lua_pushnumber(rL, asin(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_acos(DWORD rL)
{
    r_lua_pushnumber(rL, acos(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_atan(DWORD rL)
{
    r_lua_pushnumber(rL, atan(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_atan2(DWORD rL)
{
    r_lua_pushnumber(rL, atan2(r_luaL_checknumber(rL, 1), r_luaL_checknumber(rL, 2)));
    return 1;
}

static int math_ceil(DWORD rL)
{
    r_lua_pushnumber(rL, ceil(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_floor(DWORD rL)
{
    r_lua_pushnumber(rL, floor(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_fmod(DWORD rL)
{
    r_lua_pushnumber(rL, fmod(r_luaL_checknumber(rL, 1), r_luaL_checknumber(rL, 2)));
    return 1;
}

static int math_modf(DWORD rL)
{
    double ip;
    double fp = modf(r_luaL_checknumber(rL, 1), &ip);
    r_lua_pushnumber(rL, ip);
    r_lua_pushnumber(rL, fp);
    return 2;
}

static int math_sqrt(DWORD rL)
{
    r_lua_pushnumber(rL, sqrt(r_luaL_checknumber(rL, 1)));
    return 1;
}

static int math_pow(DWORD rL)//pow pow
{
    r_lua_pushnumber(rL, pow(r_luaL_checknumber(rL, 1), r_luaL_checknumber(rL, 2)));
    return 1;
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

static int math_clamp(DWORD rL)//clamp function
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

static int math_sign(DWORD rL)//sign function
{
    double v = r_luaL_checknumber(rL, 1);//todo: i hate v
    r_lua_pushnumber(rL, v > 0.0 ? 1.0 : v < 0.0 ? -1.0 : 0.0);//time to get ur brain fucked :D
    return 1;
}

static int math_max(DWORD rL)//math max function
{
    int n = r_lua_gettop(rL); /* number of arguments */
    int nn = 1;
    double dmax = r_luaL_checknumber(rL, nn);
    int i;
    for (i = 2; i <= n; i++)
    {
        double d = r_luaL_checknumber(rL, i);
        if (d > dmax)
            dmax = d;
    }
    r_lua_pushnumber(rL, dmax);//push number to r_luaL_checknumber(1)
    return 1;
}

static int math_min(DWORD  rL)//math min function (minimum)
{
    int n = r_lua_gettop(rL); /* number of arguments */
    double dmin = r_luaL_checknumber(rL, 1);
    int i;
    for (i = 2; i <= n; i++)
    {
        double d = r_luaL_checknumber(rL, i);
        if (d < dmin)//technically math_max but it checks if dmin is greater
            dmin = d;
    }
    r_lua_pushnumber(rL, dmin);
    return 1;
}
#undef PI

#define PI (3.14159265358979323846)
#define RADIANS_PER_DEGREE (PI / 180.0)

static int math_deg(DWORD rL)
{
    r_lua_pushnumber(rL, r_luaL_checknumber(rL, 1) / RADIANS_PER_DEGREE);
    return 1;
}

static int math_rad(DWORD rL)
{
    int check = 1;
    r_lua_pushnumber(rL, r_luaL_checknumber(rL, check) * RADIANS_PER_DEGREE);//multiply wow
    return 1;
}

static const unsigned char kPerlin[512] = {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99,
    37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174,
    20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41,
    55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17,
    182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110,
    79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14,
    239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24,
    72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,

    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
    120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74,
    165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65,
    25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52,
    217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213,
    119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112,
    104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199,
    106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61,
    156, 180};

static float perlin(float x, float y, float z)//imagine vector3
{
    float xflr = floorf(x);
    float yflr = floorf(y);
    float zflr = floorf(z);

    int xi = int(xflr) & 255;
    int yi = int(yflr) & 255;
    int zi = int(zflr) & 255;

    float xf = x - xflr;
    float yf = y - yflr;
    float zf = z - zflr;

    float u = fade(xf);
    float v = fade(yf);
    float w = fade(zf);

    const unsigned char* p = kPerlin;

    int a = p[xi] + yi;
    int aa = p[a] + zi;
    int ab = p[a + 1] + zi;

    int b = p[xi + 1] + yi;
    int ba = p[b] + zi;
    int bb = p[b + 1] + zi;

    return lerp(w,
        lerp(v, lerp(u, grad(p[aa], xf, yf, zf), grad(p[ba], xf - 1, yf, zf)), lerp(u, grad(p[ab], xf, yf - 1, zf), grad(p[bb], xf - 1, yf - 1, zf))),
        lerp(v, lerp(u, grad(p[aa + 1], xf, yf, zf - 1), grad(p[ba + 1], xf - 1, yf, zf - 1)),
            lerp(u, grad(p[ab + 1], xf, yf - 1, zf - 1), grad(p[bb + 1], xf - 1, yf - 1, zf - 1))));
}


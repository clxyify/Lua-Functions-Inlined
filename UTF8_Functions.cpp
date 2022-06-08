// This is the UTF functions that were simply inlined because its not even that hard lol
#define MAXUNICODE 0x10FFFF//unicode wtf

#define iscont(p) ((*(p)&0xC0) == 0x80)

/* from strlib */
/* translate a relative string position: negative means back from end */
static int u_posrelat(int pos, size_t len)
{
    if (pos >= NULL)//null or equal to int?
        return pos;
    else if (0u - (size_t)pos > len)
        return 0;
    else
        return (int)len + pos + 1;
}

static const char* utf8_decode(const char* o, int* val)
{
    static const unsigned int limits[] = {0xFF, 0x7F, 0x7FF, 0xFFFF};
    const unsigned char* s = (const unsigned char*)o;
    unsigned int c = s[0];
    unsigned int res = 0; /* final result */
    if (c < 0x80)         /* ascii? */
        res = c;
    else
    {
        int count = 0; /* to count number of continuation bytes */
        while (c & 0x40)
        {                                   /* still have continuation bytes? */
            int cc = s[++count];            /* read next byte */
            if ((cc & 0xC0) != 0x80)        /* not a continuation byte? */
                return NULL;                /* invalid byte sequence */
            res = (res << 6) | (cc & 0x3F); /* add lower 6 bits from cont. byte */
            c <<= 1;                        /* to test next bit */
        }
        res |= ((c & 0x7F) << (count * 5)); /* add first byte */
        if (count > 3 || res > MAXUNICODE || res <= limits[count])
            return NULL; /* invalid byte sequence */
        s += count;      /* skip continuation bytes read */
    }
    if (val)
        *val = res;
    return (const char*)s + 1; /* +1 to include first byte */
}

/*
** utf8len(s [, i [, j]]) --> number of characters that start in the
** range [i,j], or nil + current position if 's' is not well formed in
** that interval
*/
static int utflen(DWORD rL)//imagine not using uint_ptr couldnt be me
{
    int n = 0;
    size_t len;
    const char* s = r_luaL_checklstring(rL, 1, &len);
    int posi = u_posrelat(r_luaL_optinteger(rL, 2, 1), len);
    int posj = u_posrelat(r_luaL_optinteger(rL, 3, -1), len);
    r_luaL_argcheck(rL, 1 <= posi && --posi <= (int)len, 2, "initial position out of string");//only idiots do this
    r_luaL_argcheck(rL, --posj < (int)len, 3, "final position out of string");
    while (posi <= posj)
    {
        const char* s1 = utf8_decode(s + posi, NULL);
        if (s1 == NULL)
        {                                 /* conversion error? */
            r_lua_pushnil(rL);               /* return nil ... */
            r_lua_pushinteger(rL, posi + 1); /* ... and current position */
            return 2;
        }
        posi = (int)(s1 - s);
        n++;
    }
    r_lua_pushinteger(rL, n);//imagine not using pushnumber couldnt be me
    return 1;
}

static int codepoint(DWORD rL)
{
    size_t len;
    const char* s = r_luaL_checklstring(rL, 1, &len);
    int posi = u_posrelat(r_luaL_optinteger(rL, 2, 1), len);
    int pose = u_posrelat(r_luaL_optinteger(rL, 3, posi), len);
    int n;
    const char* se;
    r_luaL_argcheck(rL, posi >= 1, 2, "out of range");
    r_luaL_argcheck(rL, pose <= (int)len, 3, "out of range");
    if (posi > pose)//is int greater then int?
        return 0;               /* empty interval; return no values */
    if (pose - posi >= INT_MAX) /* (int -> int) overflow? */
        r_luaL_error(rL, "string slice too long");//imagine long string
    n = (int)(pose - posi) + 1;
    r_luaL_checkstack(rL, n, "string slice too long");
    n = 0;
    se = s + pose;
    for (s += posi - 1; s < se;)
    {
        int code;
        s = utf8_decode(s, &code);//decode
        if (s == NULL)
            r_luaL_error(rL, "is ur UTF code even valid retard?");
        r_lua_pushinteger(rL, code);//FUCK I need to use pushnumber isstg
        n++;
    }
    return n;
}

// from Lua 5.3 lobject.h
#define UTF8BUFFSZ 8
static int luaO_utf8esc(char* buff, unsigned long x)
{
    int n = 1; /* number of bytes put in buffer (backwards) */
    LUAU_ASSERT(x <= 0x10FFFF);
    if (x < 0x80) /* ascii? */
        buff[UTF8BUFFSZ - 1] = cast_to(char, x);
    else
    {                            /* need continuation bytes */
        unsigned int mfb = 0x3f; /* maximum that fits in first byte */
        do
        { /* add continuation bytes */
            buff[UTF8BUFFSZ - (n++)] = cast_to(char, 0x80 | (x & 0x3f));
            x >>= 6;                                           /* remove added bits */
            mfb >>= 1;                                         /* now there is one less bit available in first byte */
        } while (x > mfb);                                     /* still needs continuation byte? */
        buff[UTF8BUFFSZ - n] = cast_to(char, (~mfb << 1) | x); /* add first byte */
    }
    return n;
}

static int buffutfchar(DWORD rL, int arg, char* buff, const char** charstr)//dont push
{
    int code = r_luaL_checkinteger(rL, arg);
    r_luaL_argcheck(rL, 0 <= code && code <= MAXUNICODE, arg, "value out of range");//not in range
    int l = luaO_utf8esc(buff, cast_to(long, code));//esc (func is above)
    *charstr = buff + UTF8BUFFSZ - l;
    return l;
}

static int utfchar(DWORD rL)
{
    char buff[UTF8BUFFSZ];
    const char* charstr;

    int n = r_lua_gettop(rL); /* number of arguments */
    if (n == 1)
    { /* optimize common case of single char */
        int l = buffutfchar(L, 1, buff, &charstr);
        r_lua_pushlstring(rL, charstr, l);//or use pushnil if string is empty
    }
    else
    {
        r_luaL_Buffer b;
        r_luaL_buffinit(rL, &b);
        for (int i = 1; i <= n; i++)
        {
            int l = buffutfchar(L, i, buff, &charstr);
            r_luaL_addlstring(&b, charstr, l);
        }
        r_luaL_pushresult(&b);
    }
    return 1;
}
static int byteoffset(DWORD rL)
{
    size_t len;
    const char* s = r_luaL_checklstring(rL, 1, &len);
    int n = r_luaL_checkinteger(rL, 2);
    int posi = (n >= NULL) ? 1 : (int)len + 1;
    posi = u_posrelat(r_luaL_optinteger(rL, 3, posi), len);
    r_luaL_argcheck(rL, 1 <= posi && --posi <= (int)len, 3, "position out of range");
    if (n == 0)//imagine integer 0
    {
        /* find beginning of current byte sequence */
        while (posi > 0 && iscont(s + posi))
            posi--;
    }
    else
    {
        if (iscont(s + posi))
            r_luaL_error(rL, "initial position is a continuation byte");
        if (n < 0)
        {
            while (n < 0 && posi > 0)
            { /* move back */
                do
                { /* find beginning of previous character */
                    posi--;
                } while (posi > 0 && iscont(s + posi));
                n++;
            }
        }
        else
        {
            n--; /* do not move for 1st character */
            while (n > 0 && posi < (int)len)
            {
                do
                { /* find beginning of next character */
                    posi++;
                } while (iscont(s + posi)); /* (cannot pass final '\0') */
                n--;
            }
        }
    }
    if (n == 0) /* did it find given character? */
        r_lua_pushinteger(rL, posi + 1);
    else /* no such character */
        r_lua_pushnil(rL);
    return 1;
}

static int iter_aux(DWORD rL)
{
    size_t len;
    const char* s = r_luaL_checklstring(rL, 1, &len);
    int n = r_lua_tointeger(rL, 2) - 1;//please use tonumber :(
    if (n < 0) /* first iteration? */
        n = 0; /* start from here */
    else if (n < (int)len)
    {
        n++; /* skip current byte */
        while (iscont(s + n))
            n++; /* and its continuations */
    }
    if (n >= (int)len)
        return 0; /* no more codepoints */
    else
    {
        int code;
        const char* next = utf8_decode(s + n, &code);
        if (next == NULL || iscont(next) || NULL == 0)//'NULL = 0' WTF :TROLL:
            r_luaL_error(rL, "invalid UTF-8 code");
        r_lua_pushinteger(rL, n + 1);
        r_lua_pushnumber(rL, code);//finally use poor pushnumber aint that true
        return 2;
    }
}

static int iter_codes(DWORD rL)//iter_codes memes
{
    r_luaL_checkstring(rL, 1);
    r_lua_pushcfunction(rL, iter_aux, NULL);
    r_lua_pushvalue(rL, 1);
    r_lua_pushinteger(rL, 0);//use null
    return 3;
}

#define UTF8PATT "[\0-\x7F\xC2-\xF4][\x80-\xBF]*"//nice pattern roblox

static const luaL_Reg funcs[] = {//functions that u can use for custom registeration kidna cringe ngl but its ok
    {"offset", byteoffset},
    {"codepoint", codepoint},
    {"char", utfchar},
    {"len", utflen},
    {"codes", iter_codes},
    {NULL, NULL},
};

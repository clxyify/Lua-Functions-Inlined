//inlined luaS functions i've rewritten for roblox skidsploits
//dont say 'i skidded hash from headhunter' FYI he got it from Luau u stupid

unsigned int r_luaS_hash(const char* str, size_t len)//no duplicate, and also no state to be used as
{
    unsigned int a = 0, b = 0;
    unsigned int h = unsigned(len);

    while (len >= 32)
    {
#define rol(x, s) ((x >> s) | (x << (32 - s)))
#define mix(u, v, w) a ^= h, a -= rol(h, u), b ^= a, b -= rol(a, v), h ^= b, h -= rol(b, w)

        // should compile into fast unaligned reads
        uint32_t block[3];
        memcpy(block, str, 12);

        a += block[0];
        b += block[1];
        h += block[2];
        mix(14, 11, 25);
        str += 12;
        len -= 12;

#undef mix
#undef rol
    }

    for (size_t i = len; i > 0; --i)
        h ^= (h << 5) + (h >> 2) + (uint8_t)str[i - 1];

    return h;
}

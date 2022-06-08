// Some luaO functions ive written and modified so here you go:
void r_luaO_chunkid(DWORD rL, char* out, const char* source, size_t bufflen)//'DWORD rL' :troll:
{
    if (*source == '=')
    {
        source++; /* skip the `=' */
        size_t srclen = strlen(source);
        size_t dstlen = srclen < bufflen ? srclen : bufflen - 1;
        memcpy(out, source, dstlen);
        out[dstlen] = '\0';
    }
    else if (*source == '@')
    {
        size_t l;
        source++; /* skip the `@' */
        bufflen -= sizeof("...");
        l = strlen(source);
        strcpy(out, "");
        if (l > bufflen)
        {
            source += (l - bufflen); /* get last part of file name */
            strcat(out, "...");
        }
        strcat(out, source);
    }
    else
    {                                         /* out = [string "string"] */
        size_t len = strcspn(source, "\n\r"); /* stop at first newline */
        bufflen -= sizeof("[string \"...\"]");
        if (len > bufflen)
            len = bufflen;
        strcpy(out, "[string \"");
        if (source[len] != '\0')
        { /* must truncate? */
            strncat(out, source, len);
            strcat(out, "...");
        }
        else
            strcat(out, source);
        strcat(out, "\"]");
    }
}

int r_luaO_str2d(const char* s, double* result)
{
    char* endptr;
    *result = luai_str2num(s, &endptr);
    if (endptr == s)
        return 0;                         /* conversion failed */
    if (*endptr == 'x' || *endptr == 'X') /* maybe an hexadecimal constant? */
        *result = cast_num(strtoul(s, &endptr, 16));
    if (*endptr == '\0')
        return 1; /* most common case */
    while (isspace(cast_to(unsigned char, *endptr)))
        endptr++;
    if (*endptr != '\0')
        return 0; /* invalid trailing characters? */
    return 1;
}

const TValue luaO_nilobject_ = {{NULL}, {0}, r_LUA_TNIL};//global header

int r_luaO_log2(unsigned int x)//imagine unsigned
{
    static const uint8_t log_2[256] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
    int l = -1;
    while (x >= 256)//imagine equal to 256
    {
        l += 8;
        x >>= 8;
    }
    return l + log_2[x];
}

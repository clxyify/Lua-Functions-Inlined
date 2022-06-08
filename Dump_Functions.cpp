// Dump functions kinda works:

inline bool safejson(char ch)//ayo inline omg!1!1!
{
    return unsigned(ch) < 128 && ch >= 32 && ch != '\\' && ch != '\"';
}

static void dumpref(FILE* f, GCObject* o)
{
    fprintf(f, "\"%p\"", o);
}

static void dumpstringdata(FILE* f, const char* data, size_t len)//dumpstringdata lol
{
    for (size_t i = 0; i < len; ++i)
        fputc(safejson(data[i]) ? data[i] : '?', f);
}

static void dumpstring(FILE* f, TString* ts)//dumpstrings probably use std::cout since pro
{
    size_t* len;//len
    fprintf(f, "{\"type\":\"string\",\"cat\":%d,\"size\":%d,\"data\":\"", ts->memcat, int(sizestring(ts->len)));
    dumpstringdata(f, ts->data, ts->len);
    fprintf(f, "\"}");
}

static void dumpproto(FILE* f, Proto* p)//uses lua files
{
    size_t size = sizeof(Proto) + sizeof(Instruction) * p->sizecode + sizeof(Proto*) * p->sizep + sizeof(TValue) * p->sizek + p->sizelineinfo +
                  sizeof(LocVar) * p->sizelocvars + sizeof(TString*) * p->sizeupvalues;

    fprintf(f, "{\"type\":\"proto\",\"cat\":%d,\"size\":%d", p->memcat, int(size));

    if (p->source)
    {
        fprintf(f, ",\"source\":\"");
        dumpstringdata(f, p->source->data, p->source->len);
        fprintf(f, "\",\"line\":%d", p->abslineinfo ? p->abslineinfo[0] : 0);
    }

    if (p->sizek)
    {
        fprintf(f, ",\"constants\":[");
        dumprefs(f, p->k, p->sizek);
        fprintf(f, "]");
    }

    if (p->sizep)
    {
        fprintf(f, ",\"protos\":[");
        for (int i = 0; i < p->sizep; ++i)
        {
            if (i != 0)
                fputc(',', f);
            dumpref(f, obj2gco(p->p[i]));
        }
        fprintf(f, "]");
    }

    fprintf(f, "}");
}


static void dumpclosure(FILE* f, Closure* cl)//dumps closure
{
    fprintf(f, "{\"type\":\"function\",\"cat\":%d,\"size\":%d", cl->memcat,
        cl->isC ? int(sizeCclosure(cl->nupvalues)) : int(sizeLclosure(cl->nupvalues)));

    fprintf(f, ",\"env\":");
    dumpref(f, obj2gco(cl->env));

    if (cl->isC)
    {
        if (cl->nupvalues)
        {
            fprintf(f, ",\"upvalues\":[");
            dumprefs(f, cl->c.upvals, cl->nupvalues);
            fprintf(f, "]");
        }
    }
    else
    {
        fprintf(f, ",\"proto\":");
        dumpref(f, obj2gco(cl->l.p));
        if (cl->nupvalues)
        {
            fprintf(f, ",\"upvalues\":[");
            dumprefs(f, cl->l.uprefs, cl->nupvalues);
            fprintf(f, "]");
        }
    }
    fprintf(f, "}");
}

static void dumpudata(FILE* f, Udata* u)//dumps udata
{
    fprintf(f, "{\"type\":\"userdata\",\"cat\":%d,\"size\":%d,\"tag\":%d", u->memcat, int(sizeudata(u->len)), u->tag);

    if (u->metatable)
    {
        fprintf(f, ",\"metatable\":");
        dumpref(f, obj2gco(u->metatable));
    }
    fprintf(f, "}");
}

static void dumpthread(FILE* f, lua_State* th)//dumps thread u should use lua_state*
{
    size_t size = sizeof(DWORD) + sizeof(TValue) * th->stacksize + sizeof(CallInfo) * th->size_ci;

    fprintf(f, "{\"type\":\"thread\",\"cat\":%d,\"size\":%d", th->memcat, int(size));

    fprintf(f, ",\"env\":");
    dumpref(f, obj2gco(th->gt));

    Closure* tcl = 0;
    for (CallInfo* ci = th->base_ci; ci <= th->ci; ++ci)
    {
        if (ttisfunction(ci->func))
        {
            tcl = clvalue(ci->func);
            break;
        }
    }

    if (tcl && !tcl->isC && tcl->l.p->source)
    {
        Proto* p = tcl->l.p;

        fprintf(f, ",\"source\":\"");
        dumpstringdata(f, p->source->data, p->source->len);
        fprintf(f, "\",\"line\":%d", p->abslineinfo ? p->abslineinfo[0] : 0);
    }

    if (th->top > th->stack)
    {
        fprintf(f, ",\"stack\":[");
        dumprefs(f, th->stack, th->top - th->stack);
        fprintf(f, "]");
    }
    fprintf(f, "}");
}

static void dumpproto(FILE* f, Proto* p)//dumps proto
{
    size_t size = sizeof(Proto) + sizeof(Instruction) * p->sizecode + sizeof(Proto*) * p->sizep + sizeof(TValue) * p->sizek + p->sizelineinfo +
                  sizeof(LocVar) * p->sizelocvars + sizeof(TString*) * p->sizeupvalues;

    fprintf(f, "{\"type\":\"proto\",\"cat\":%d,\"size\":%d", p->memcat, int(size));

    if (p->source)
    {
        fprintf(f, ",\"source\":\"");
        dumpstringdata(f, p->source->data, p->source->len);
        fprintf(f, "\",\"line\":%d", p->abslineinfo ? p->abslineinfo[0] : 0);
    }

    if (p->sizek)
    {
        fprintf(f, ",\"constants\":[");
        dumprefs(f, p->k, p->sizek);
        fprintf(f, "]");
    }

    if (p->sizep)
    {
        fprintf(f, ",\"protos\":[");
        for (int i = 0; i < p->sizep; ++i)
        {
            if (i != 0)
                fputc(',', f);
            dumpref(f, obj2gco(p->p[i]));
        }
        fprintf(f, "]");
    }

    fprintf(f, "}");
}

static void dumpupval(FILE* f, UpVal* uv)
{
    fprintf(f, "{\"type\":\"upvalue\",\"cat\":%d,\"size\":%d", uv->memcat, int(sizeof(UpVal)));

    if (iscollectable(uv->v))
    {
        fprintf(f, ",\"object\":");
        dumpref(f, gcvalue(uv->v));
    }
    fprintf(f, "}");
}

static void dumpobj(FILE* f, GCObject* o)
{
    switch (o->gch.tt)
    {
    case LUA_TSTRING:
        return dumpstring(f, gco2ts(o));

    case LUA_TTABLE:
        return dumptable(f, gco2h(o));

    case LUA_TFUNCTION:
        return dumpclosure(f, gco2cl(o));

    case LUA_TUSERDATA:
        return dumpudata(f, gco2u(o));

    case LUA_TTHREAD:
        return dumpthread(f, gco2th(o));

    case LUA_TPROTO:
        return dumpproto(f, gco2p(o));

    case LUA_TUPVAL:
        return dumpupval(f, gco2uv(o));

    default:
        LUAU_ASSERT(0);
    }
}

static bool dumpgco(void* context, lua_Page* page, GCObject* gco)
{
    FILE* f = (FILE*)context;

    dumpref(f, gco);
    fputc(':', f);
    dumpobj(f, gco);
    fputc(',', f);
    fputc('\n', f);

    return false;
}

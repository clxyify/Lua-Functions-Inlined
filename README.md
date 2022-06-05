# Lua-Functions-Inlined

Inlined Lua API Functions that are Compatible with most sources.

Instead of using `lua_State* L`, we replaced it with double-word `DWORD rL`.

Then instead of using lua's shitty functions, we use our own inlined ones!

Must include `Headers.hpp` or any **Lua Files** otherwise functions will error.

**If any definition error occurs, include some lua files, should fix the error.**

**Not all functions work, because I didn't really work on some functions.**

# Lua-Functions-Inlined

Inlined Lua API Functions that are Compatible with most sources.

Instead of using `lua_State* L`, we replaced it with double-word `DWORD rL`.

Then instead of using lua's shitty functions, we use our own inlined ones!

Must include `Headers.hpp` or any **Lua Files** otherwise functions will error.

**If any definition error occurs, include some lua files, should fix the error.**

**Not all functions work, because I didn't really work on some functions.**

# What-is-inlining?

The __inline Keyword for Inline Functions. An inline function is a function for which the compiler replaces a call to the function with the code for the function itself.

Roblox destroyed the usage of functions, into inline, and this repo fixes that.

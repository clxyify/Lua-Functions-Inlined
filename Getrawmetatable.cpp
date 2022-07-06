/* compataible and works for luau's compiler based exploits! */
int __cdecl getrawmetatable(int a1) /* rbx state */
{
  int top = 20;
  int base = 8;
  DWORD *v1;
  int result;

  v1 = (DWORD *)(**(DWORD **)(a1 + base) + base * (*(DWORD *)(*(DWORD *)(a1 + base) + 12) != base) + base);
  if ( (DWORD *)*v1 == v1 ) /* enc */
    return 0;
  **(DWORD **)(a1 + top) = *v1 - (DWORD)v1;
  *(DWORD *)(*(DWORD *)(a1 + top) + 12) = 6;
  result = 1;
  *(DWORD *)(a1 + top) += 16;
  return result; /* return null or 1 your choice */
}

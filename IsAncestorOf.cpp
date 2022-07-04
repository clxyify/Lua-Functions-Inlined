/* checks if its a 'ancestor' mostly defined as a char def */
/* works with any execution method and it's 100% inlined! */
//to update this scan this pattern->55 8B EC 64 A1 ?? ?? ?? ?? 6A ?? 68 ?? ?? ?? ?? 50 64 89 25 ?? ?? ?? ?? 53 56 57 8B 45 08 85 C0

char __thiscall ancestorcall(_DWORD *this, _DWORD *a2)
{
  _DWORD *v2; // eax

  v2 = a2;
  if ( !a2 )
    return 0;
  while ( 1 )
  {
    v2 = (_DWORD *)v2[12];
    if ( v2 == this )
      break;
    if ( !v2 )
      return 0;
  }
  return 1;
}

char __thiscall IsAncestorOf(void *this, int a2, volatile signed __int32 *a3)
{
  char v3; // bl
  void *v4; // eax

  if ( a2 )
  {
    v4 = *(void **)(a2 + 48);
    if ( v4 == this )
      v3 = 1;
    else
      v3 = ancestorcall(v4);
  }
  else
  {
    v3 = 0;
  }
  if ( a3 )
  {
    if ( !_InterlockedExchangeAdd(a3 + 1, 0xFFFFFFFF) )
    {
      (**(void (__thiscall ***)(volatile signed __int32 *))a3)(a3);
      if ( !_InterlockedDecrement(a3 + 2) )
        (*(void (__thiscall **)(volatile signed __int32 *))(*a3 + 4))(a3);
    }
  }
  return v3;
}

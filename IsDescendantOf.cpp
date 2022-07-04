/*IsDescendantOf inlined*/

char __thiscall sub_6E48B0(_DWORD *this, int a2)
{
  int v2; // eax

  v2 = this[12];
  if ( a2 == v2 )
    return 1;
  while ( v2 )
  {
    v2 = *(_DWORD *)(v2 + 48);
    if ( a2 == v2 )
      return 1;
  }
  return 0;
}
char __thiscall IsDescendantOf(_DWORD *this, int a2, volatile signed __int32 *a3)
{
  int v3; // ecx
  char v4; // bl

  v3 = this[12];
  if ( a2 == v3 )
  {
    v4 = 1;
  }
  else if ( v3 )
  {
    v4 = sub_6E48B0(a2);
  }
  else
  {
    v4 = 0;
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
  return v4;
}

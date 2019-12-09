#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "luaclass.h"


int main(){
  LuaObj lua;
  char s[100]="aaa";
  int i=42;
  double f = 100500;
  lua.LoadFile("scr.lua");
  lua.Run();
  
  lua.SetVal("z", 100500);
  lua.Call("func2()");
  lua.Call("func(100, %i, 'fff'):%s,%i", 200, s, &i);
  lua.GetStr("z", s, 100);
  printf("z=%s\n", s);
  printf("RESULT = %i, %s\n", i, s);
  printf("\n");
}

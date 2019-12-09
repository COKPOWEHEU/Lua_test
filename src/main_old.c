#include <math.h>
#include <stdio.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>

void stackDump( lua_State *L ){
  printf("Stack={\n");
  int i;
  int top = lua_gettop(L); /* глубина стека */
  for(i = 1; i <= top; i++){ /* повторяет для каждого уровня */
    int t = lua_type(L, i);
    printf("  ");
    switch( t ){
      case LUA_TSTRING: /* строки */
        printf("%s", lua_tostring(L, i));
        break;
      case LUA_TBOOLEAN: /* булевы значения */
        printf(lua_toboolean(L, i) ? "true" : "false");
        break;
      case LUA_TNUMBER: /* числа */
        printf("%g", lua_tonumber(L, i));
        break;
      default: /* другие значения */
        printf("%s", lua_typename(L, t));
        break;
    }
    printf(" "); /* помещает разделитель */
  }
  printf("\n}//stack end\n"); /* конец листинга */
}

int main(int argc, char **argv){
  
  char lua_code[]=
  "x = 10 \n"
  "print(\"x=\",x)\n"
  "\n"
  "function func()\n"
  "  x = x+1\n"
  "  print(\"func:\",x)\n"
  "end\n"
  ;
  
  lua_State *L = luaL_newstate();
  if( L == NULL ){printf("Can not create LUA\n"); return -1;}
  luaL_openlibs(L);
  
  luaL_loadstring(L, lua_code);
  
  stackDump(L);
  lua_pcall(L, 0, 0, 0);
  
  lua_pushnumber(L, 100);
  lua_setglobal(L, "x");
  
  lua_getglobal(L, "func");
  stackDump(L);
  lua_pcall(L, 0, 0, 0);
  lua_pop(L, 1);
  stackDump(L);
  
  
  /*for(x=min; x < maxx; x+=diff){
    lua_pushnumber(L, x);
    lua_setglobal(L, "x");
    
    luaL_loadstring(L, func);
    lua_pcall(L, 0, 0, 0);
  
    lua_getglobal(L, "y");
    y = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    AppendLin(x, y, &b, &k);
    if( y < miny )miny = y;
    if( y > maxy )maxy = y;
  }
  printf("y = %g*x %c %g\n", k, (b>=0?'+':'-'), fabs(b));
*/
  
  lua_close(L);
}

#ifndef __LUA_CLASS_H__
#define __LUA_CLASS_H__
#ifdef cplusplus
extern "C"{
#endif
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>

class LuaObj{
private:
  lua_State *L;
  int LastErr;
  int LuaErr;
public:
  LuaObj();
  ~LuaObj();
  void LuaInit();
  int GetErr();
  char* GetStrErr();
  void ClearErr();
  int LoadFile(char filename[]);
  int LoadStr(char str[]);
  
  void Run();
  int GetInt(char name[]);
  double GetFloat(char name[]);
  void GetStr(char name[], char *res, size_t size);
  void SetVal(char name[], int val);
  void SetVal(char name[], double val);
  void SetVal(char name[], char *val);
  
  int Call(char fmt[], ...);
  
  void StackDump();
};

#ifdef cplusplus
}
#endif
#endif

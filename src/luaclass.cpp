#include "luaclass.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define LUAERR_CREATE -1


LuaObj::LuaObj(){
  L = luaL_newstate();
  if(L == NULL){
    LastErr = LUAERR_CREATE;
    return;
  }
  luaL_openlibs(L);
}

LuaObj::~LuaObj(){
  lua_close(L);
}

void LuaObj::LuaInit(){
  if(L != NULL)lua_close(L);
  LastErr = 0;
  L = luaL_newstate();
  if(L == NULL){
    LastErr = LUAERR_CREATE;
    return;
  }
  luaL_openlibs(L);
}

int LuaObj::GetErr(){
  return LastErr;
}

char* LuaObj::GetStrErr(){
  return NULL;
}

void LuaObj::ClearErr(){
  if(LastErr != LUAERR_CREATE)LastErr = 0;
}

int LuaObj::LoadFile(char filename[]){
  int res = luaL_loadfile(L, filename);
  if(res == LUA_OK)return 0;
  //TODO
  if(res == LUA_ERRSYNTAX)LastErr = 1;
  if(res == LUA_ERRMEM)LastErr = 2;
  if(res == LUA_ERRGCMM)LastErr = 3;
  return LastErr;
}

int LuaObj::LoadStr(char str[]){
  int res = luaL_loadstring(L, str);
  if(res == LUA_OK)return 0;
  //TODO
  if(res == LUA_ERRSYNTAX)LastErr = 1;
  if(res == LUA_ERRMEM)LastErr = 2;
  if(res == LUA_ERRGCMM)LastErr = 3;
  return LastErr;
}

void LuaObj::SetVal(char name[], int val){
  lua_pushinteger(L, val);
  lua_setglobal(L, name);
}
void LuaObj::SetVal(char name[], double val){
  lua_pushnumber(L, val);
  lua_setglobal(L, name);
}
void LuaObj::SetVal(char name[], char *val){
  lua_pushstring(L, val);
  lua_setglobal(L, name);
}

int LuaObj::GetInt(char name[]){
  lua_getglobal(L, name);
  int res = lua_tonumber(L, -1);
  lua_pop(L, 1);
  return res;
}
double LuaObj::GetFloat(char name[]){
  lua_getglobal(L, name);
  double res = lua_tonumber(L, -1);
  lua_pop(L, 1);
  return res;
}
void LuaObj::GetStr(char name[], char *res, size_t size){
  lua_getglobal(L, name);
  const char *val = lua_tostring(L, -1);
  lua_pop(L, 1);
  strncpy(res, (char*)val, size);
}

void LuaObj::StackDump(){
  printf("Stack={\n");
  int i;
  int top = lua_gettop(L);
  for(i = 1; i <= top; i++){
    int t = lua_type(L, i);
    printf("  ");
    switch( t ){
      case LUA_TSTRING:
        printf("%s", lua_tostring(L, i));
        break;
      case LUA_TBOOLEAN:
        printf(lua_toboolean(L, i) ? "true" : "false");
        break;
      case LUA_TNUMBER:
        printf("%g", lua_tonumber(L, i));
        break;
      default:
        printf("%s", lua_typename(L, t));
        break;
    }
    printf(" ");
  }
  printf("\n}//stack end\n");
}

int LuaObj::Run(){
  int res = lua_pcall(L, 0, 0, 0);
  if(res == LUA_OK)return 0;
  return -1;
}

int LuaObj::Call(char fmt[], ...){
  char *tmp = strdup(fmt);
  char *func = tmp;
  char *en = strchr(tmp, '(');
  char *retval = NULL;
  
  va_list argp;
  va_start(argp, fmt);
  
  if(en == NULL){
    free(tmp);
    return -1;
  }
  en[0] = 0;
  
  //имя функции
  lua_getglobal(L, func);
  if(!lua_isfunction(L, 1)){
    lua_pop(L, 1);
    free(tmp);
    return -1; //TODO: lastErr
  }
  
  //передача параметров в функцию
  int argnum = 0;
  char *str;
  while(retval==NULL){
    str = en+1;
    while(isspace(str[0]))str++;
    en = str;
    while(!isspace(en[0]) && (en[0]!=',') && (en[0]!=')') && (en[0]!=0)){
      if((en[0] == '"')||(en[0]=='\'')){
        char quota = en[0];
        do{
          en = strchr(en+1, quota);
          //TODO: убрать экранирование кавычки (сдвиг строки)
        }while(en[-1]=='\\');
        en[0]=0;
      }
      en++;
    }
    if(en[0] == ')')retval=en;
    en[0] = 0;
    
    argnum++;
    if(str[0] == '%'){
      if(str[1] == 's'){
        char *val = va_arg(argp, char*);
        lua_pushstring(L, val);
      }else if(str[1] == 'i'){
        int val = va_arg(argp, int);
        lua_pushnumber(L, val);\
      }else if((str[1] == 'f')||((str[1]=='l')&&(str[1]=='f'))){
        double val = va_arg(argp, double);
        lua_pushnumber(L, val);
      }else{
        //TODO printf("Unknown\n");
      }
    }else if((str[0] == '"') || (str[0]=='\'')){
      lua_pushstring(L, str+1);
    }else if(str[0] == 0){
      if(retval==NULL)lua_pushnil(L);
        else argnum--;
    }else{
      if((strchr(str,'.')!=NULL) || (strchr(str, 'e')!=NULL) || (strchr(str, 'E')!=NULL)){
        double val = strtod(str, NULL);
        lua_pushnumber(L, val);
      }else{
        int val = atoi(str);
        lua_pushnumber(L, val);
      }
    }
  }
  
  //подсчет возвращаемых значений
  int retnum = 0;
  char *pos = retval;
  while( (pos=strchr(pos+1, '%'))!=NULL)retnum++;

  //Вызов
  int funcres = lua_pcall(L, argnum, retnum, 0);
  if(funcres != 0){
    printf("ERR = %i\n", funcres);
  }

  //возврат параметров из функции
  argnum=0;
  while( (retval=strchr(retval+1, '%'))!=NULL){
    argnum++;
    if(retval[1] == 'i'){
      *va_arg(argp, int*) = lua_tonumber(L, argnum);
    }else if(retval[1] == 'f'){
      *va_arg(argp, float*) = lua_tonumber(L, argnum);
    }else if((retval[1] == 'l')||(retval[2]=='f')){
      *va_arg(argp, double*) = lua_tonumber(L, argnum);
    }else if(retval[1] == 's'){
      const char *res = lua_tostring(L, argnum);
      strcpy(va_arg(argp, char*), res);
    }
  }
  va_end(argp);
  
  lua_pop(L, argnum);
  free(tmp);
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//lua头文件
#include "test_lua.h"
#include "Test.h"
#include "Network.h"
#include <assert.h>
#include <string.h>

typedef struct userdata{ void*p; } userdata;
typedef struct app{
	lua_State* L;
}app; 
app App;


#define err_exit(num,fmt,args)  \
    do{printf("[%s:%d]"fmt"\n",__FILE__,__LINE__,##args);exit(num);} while(0)
#define err_return(num,fmt,args)  \
    do{printf("[%s:%d]"fmt"\n",__FILE__,__LINE__,##args);return(num);} while(0)

//lua中调用的c函数定义,实现加法
int csum(lua_State* l)
{
    int a = lua_tointeger(l,1) ;
    int b = lua_tointeger(l,2) ;
    lua_pushinteger(l,a+b) ;
    return 1 ;
}

static int doError(lua_State * L)
{
	printf("doError:%s\n", lua_tostring(L, -1));
	//return lua_tostring(L, -1);
	return 1;
}

static int newNetwork(lua_State* L)
{
	//lua_remove(L, -1);
	Network* obj = new Network();
	//lua_pushlightuserdata(L, obj);
	userdata* data = static_cast<userdata*>(lua_newuserdata(L, sizeof(userdata)));
	data->p = obj;
	
	luaL_newmetatable(L, "Network");
	//assert(lua_isnil(L, -1) == 0);
	lua_setmetatable(L, -2);
	
	printf("userdata addr:%x", (int)data);
	//lua_pushstring(L, "network");
	return 1;
}

static int exeFunc(lua_State* L)
{
	printf("exec func\n");
	//lua_pop(L, 1);
	userdata *data = static_cast<userdata*>(lua_touserdata(L, -2));
	const char* str = lua_tostring(L, -1);
	Network* obj = (Network*)data->p;
	obj->write(str);
	lua_pop(L, 2);
	return 0;
}

static int gcObj(lua_State* L)
{
	printf("gc\n");
	userdata *data = static_cast<userdata*>(lua_touserdata(L, -1));
	Network* obj = (Network*)data->p;
	delete obj;
	data->p = NULL;
	lua_pop(L, 1);
	return 0;
}

void registerClass(lua_State* L)
{
	lua_pushstring(L, "Network");
	lua_gettable(L, LUA_GLOBALSINDEX);
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushstring(L, "Network");
		lua_pushvalue(L, -2);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	int ginx = lua_gettop(L);

	luaL_newmetatable(L, "Network");
	int metatable = lua_gettop(L);

	lua_pushstring(L, "__index");
	//lua_pushcfunction(L, exeFunc);
	lua_pushvalue(L, ginx);
	lua_settable(L, metatable);

	lua_pushstring(L, "__call");
	lua_pushcfunction(L, newNetwork);
	lua_settable(L, metatable);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, gcObj);
	lua_settable(L, metatable);

	lua_pushstring(L, "write");
	lua_pushcfunction(L, exeFunc);
	lua_settable(L, ginx);

	lua_setmetatable(L, ginx);

	lua_pop(L, 1);
}

void dumpStack(lua_State* L)
{
	printf("stack size:%d\n", lua_gettop(L));
}

static int getCallDepth(lua_State* L)
{
	int i = 0;
	lua_Debug ar;
	for(;lua_getstack(L, i + 1, &ar) != 0; i++);
	return i;
}

void readFromNetwork(char* str)
{
	lua_State* L = App.L;
	lua_getglobal(L, "read");
	lua_pushstring(L, str);
	lua_pcall(L, 1, 0, 0);
}

static void luaHook(lua_State* L, lua_Debug* ar)
{
	int depth = getCallDepth(L);
	lua_getstack(L, 0, ar);
	lua_getinfo(L, "lnS", ar);
	if(ar->event == LUA_HOOKLINE)
	{
		printf("line hook(%d), %s:%d\n", depth, ar->short_src, ar->currentline);
	}else if (ar->event == LUA_HOOKCALL)
	{
		printf("Func hook(%d), %s:%d\n", depth, ar->short_src, ar->currentline);
	}
	
}

static void funcHook(lua_State* L, lua_Debug* ar)
{
	printf("func hook, %s:%d\n", ar->short_src, ar->currentline);
}

int main(int argc,char** argv)
{
    lua_State * l = luaL_newstate() ;        //创建lua运行环境
	App.L = l;
	dumpStack(l);
	luaL_openlibs(l);

	int mask = lua_gethookmask(l);
	lua_sethook(l, luaHook, mask | LUA_MASKLINE | LUA_MASKCALL, 0);
	
    //if ( l == NULL ) err_return(-1,"luaL_newstat() failed");
	lua_pushstring(l, "ggg");
	lua_newtable(l);
	lua_pushstring(l, "keys");
	lua_pushstring(l, "values liujian");
	lua_settable(l, -3);
	lua_settable(l, LUA_GLOBALSINDEX);


	tolua_testlib_open(l);
	registerClass(l);
	dumpStack(l);
	int ret = luaL_loadfile(l,"main.lua") ;
	if(ret!=0) std::cout<< "luaL_loadfile failed";

	ret = lua_pcall(l,0,0,0);
	if ( ret != 0 ) std::cout<<"lua_pcall error\n";

	lua_pushcfunction(l, doError);
	lua_getglobal(l, "main");
	if(lua_isnil(l, -1)) std::cout<<"main is nil\n";
	ret = lua_pcall(l,0,LUA_MULTRET,-2);
	if ( ret != 0 ) std::cout<< "lua_pcall failed, code:" << ret << std::endl;

    /*int ret = 0 ;
	int a,b;
    ret = luaL_loadfile(l,"func.lua") ;      //加载lua脚本文件
	
    //if ( ret != 0 ) err_return(-1,"luaL_loadfile failed") ;
    ret = lua_pcall(l,0,0,0) ;
    if ( ret != 0 ) err_return(-1,"lua_pcall failed:%s",lua_tostring(l,-1)) ;

    lua_getglobal(l,"width");              //获取lua中定义的变量
    lua_getglobal(l,"height");
    printf("height:%ld width:%ld\n",lua_tointeger(l,-1),lua_tointeger(l,-2)) ;
    lua_pop(l,1) ;                        //恢复lua的栈

    a = 11;
    b = 12;
    lua_getglobal(l,"sum");               //调用lua中的函数sum
    lua_pushinteger(l,a) ;
    lua_pushinteger(l,b) ;
    ret = lua_pcall(l,2,1,0) ;
    if ( ret != 0 ) err_return(-1,"lua_pcall failed:%s",lua_tostring(l,-1)) ;
    printf("sum:%d + %d = %ld\n",a,b,lua_tointeger(l,-1)) ;
    lua_pop(l,1) ;

    const char str1[] = "hello" ;
    const char str2[] = "world" ;
    lua_getglobal(l,"mystrcat");          //调用lua中的函数mystrcat
    lua_pushstring(l,str1) ;
    lua_pushstring(l,str2) ;
    ret = lua_pcall(l,2,1,0) ;
    if ( ret != 0 ) err_return(-1,"lua_pcall failed:%s",lua_tostring(l,-1)) ;
    printf("mystrcat:%s%s = %s\n",str1,str2,lua_tostring(l,-1)) ;
    lua_pop(l,1) ;

    lua_pushcfunction(l,csum) ;         //注册在lua中使用的c函数
    lua_setglobal(l,"csum") ;           //绑定到lua中的名字csum

    lua_getglobal(l,"mysum");           //调用lua中的mysum函数，该函数调用本程序中定义的csum函数实现加法
    lua_pushinteger(l,a) ;
    lua_pushinteger(l,b) ;
    ret = lua_pcall(l,2,1,0) ;
    if ( ret != 0 ) err_return(-1,"lua_pcall failed:%s",lua_tostring(l,-1)) ;
    printf("mysum:%d + %d = %ld\n",a,b,lua_tointeger(l,-1)) ;
    lua_pop(l,1) ;


	lua_newtable(l);
	int tinx = lua_gettop(l);
	lua_pushvalue(l, -1);
	int ctinx = lua_gettop(l);
	lua_pushstring(l, "hello");
	lua_pushstring(l, "liujian");
	lua_settable(l, tinx);

	lua_pushstring(l, "hello");
	lua_gettable(l, ctinx);
	
	printf("get from table copy, %s,", lua_tostring(l, -1));*/
	readFromNetwork("read from network");
	dumpStack(l);
	lua_gc(l, LUA_GCCOLLECT, 0);

	printf("%d\n", strspn(" p xxx", " "));
	printf("%d\n", strcspn("p xxx", " "));
	getchar();
	lua_close(l) ;                     //释放lua运行环境
	
    return 0 ;
}
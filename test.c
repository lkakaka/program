#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

lua_State* L;

int luaadd ( int x, int y )
{
	int sum;

	/* the function name */
	lua_getglobal(L, "add");

	/* the first argument */
	lua_pushnumber(L, x);

	/* the second argument */
	lua_pushnumber(L, y);

	/* call the function with 2
	arguments, return 1 result */
	  //说明调用的函数是带有2个参数和1个返回值
	lua_call(L, 2, 1);

	/* get the result */
	sum = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return sum;
}

static int average(lua_State *L)  
{  
    //返回栈中元素的个数  
    int n = lua_gettop(L);  
    double sum = 0;  
    int i;  
    for (i = 1; i <= n; i++)  
    {  
        if (!lua_isnumber(L, i))   
        {  
            lua_pushstring(L, "Incorrect argument to 'average'");  
            lua_error(L);  
        }  
        sum += lua_tonumber(L, i);  
    }  
    /* push the average */  
    lua_pushnumber(L, sum / n);  
    /* push the sum */  
    lua_pushnumber(L, sum);  
      
    /* return the number of results */  
    return 2;  
}

int main (void)
{
	char buff[256];
	int error, sum;
	L = lua_open();	/* opens Lua */
	//luaopen_base(L);			/* opens the basic library */
	//luaopen_table(L);			/* opens the table library */
	//luaopen_io(L);			/* opens the I/O library */
	//luaopen_string(L);		/* opens the string lib. */
	//luaopen_math(L);			/* opens the math lib. */
	luaL_openlibs(L);
	luaL_dofile(L, ".\\test.lua");
	sum = luaadd(10, 15);
	/* print the result */
   printf( "The sum is %d", sum );

	/*while (fgets(buff, sizeof(buff), stdin) != NULL) {
		error = luaL_loadbuffer(L, buff, strlen(buff),
					"line") || lua_pcall(L, 0, 0, 0);
		if (error) {
			fprintf(stderr, "%s", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}*/

	lua_close(L);
	return 0;
}

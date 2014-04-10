module("luac", package.seeall)

function luaAccessC()
	local test = Test()
	test:sayHello()
	--test:delete()
	--test:sayHello()
	test:sendMsg("liujian test lua access c++ obj\n")
end

--print("test lua access c++ obj")
--main()
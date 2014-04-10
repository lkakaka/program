require "tolua"

function main()
	luac.luaAccessC()

	local net = Network()
	if not net then
		print("net is nil\n")
	else
		print(tostring(net))
	end

	net:write("network write");
	--print("lua addr:" .. net)

	print("\n" .. ggg["keys"])
end

function main1()
	print("main1\n")
end

function read(str)
	print("lua reveive message:" .. str);
end

--main()
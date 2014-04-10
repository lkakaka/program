#include "Test.h"


Test::Test(void)
{
}


Test::~Test(void)
{
}

void Test::sayHello()
{
	std::cout << "hello";
}

void Test::sendMsg(std::string str)
{
	std::cout << str.c_str();
}

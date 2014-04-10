#pragma once
#include <iostream>
extern void readFromNetwork(char* str);

class Network
{
public:
	Network(void);
	~Network(void);

	void write(const char* str);
	void read();
};


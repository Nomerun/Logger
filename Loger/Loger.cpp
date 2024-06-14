#include "LoggerMain.h"
#include <iostream>
#include <thread>
int n = 0;

void hell()
{
	n++;
}

void out()
{
	std::cout << n;
}

int main()
{
	LOG_FILE_DEFAULT();
	LOG_TRACE("nnn");
	LOG_FILE_CUSTOM("NEW");
	LOG_DEBUG("123");
	LOG_FILE_CUSTOM("NEW2");
	LOG_DEBUG("123443");

	//std::thread th(hell);
	//std::thread th2(hell);
	//th.join();
	//th2.join();

	return 0;
}

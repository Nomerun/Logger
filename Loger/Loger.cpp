#include "LoggerMain.h"
#include <iostream>
#include <thread>
int n = 0;


int main()
{
	LOG_FILE_DEFAULT();
	Logger::ifError(__FILE__, __LINE__, "%s----%d", "nenennee", 123);
	LOG_FILE_CUSTOM("NEW");
	LOG_FILE_CUSTOM("NOTTE");
	LOG_FILE_CUSTOM("NEW");
	LOG_TRACE("nenennee");
	LOG_DEBUG("debagigigigi");
	LOG_CLOSE_CUSTOM("NEW");
	LOG_DEBUG("deb23235235235235235235i");
	LOG_TRACE("123");

	return 0;
}

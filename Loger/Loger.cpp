#include "LoggerMain.h"

//void logTest(int n)
//{
//	Logger::ifTrace(1, 1, "%d", n);
//}

int main()
{
	LOG_FILE_DEFAULT();
	LOG_TRACE("1111");
	LOG_FILE_CUSTOM("HUINA");
	LOG_CONSOLE();
	//logTest(1);
	LOG_TRACE("ninini");
	LOG_ERROR("123");

	return 0;
}

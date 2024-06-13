#include "LoggerMain.h"



int main()
{
	LOG_FILE_DEFAULT();
	LOG_TRACE("1111");
	LOG_FILE_CUSTOM("HUINA");
	LOG_CONSOLE();
	LOG_TRACE("ninini");
	LOG_ERROR("123");

	return 0;
}

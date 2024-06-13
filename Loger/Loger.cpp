#include "LoggerMain.h"

void logTest(int n)
{
	Logger::ifTrace(1, 1, "%d", n);
}

int main()
{
	Logger::fileName("Hu");
	logTest(1);

	return 0;
}

#pragma once
#include <stdio.h>

enum LogPriority
{
	Trace, Debug, Info, Warn, Error, Critical
};

class Logger
{
private:
	static LogPriority prior;

public:
	static void setPriority(LogPriority newPriority)
	{
		prior = newPriority;
	}

	template<typename... Args>
	static void ifTrace(const char* message, Args...args)
	{
		if (prior <= Trace)
		{
			printf("[Trace]");
			printf(message, args...);
			printf("\n");
		}
	}

	template<typename... Args>
	static void ifDebug(const char* message, Args...args)
	{
		if (prior <= Debug)
		{
			printf("[Debug]");
			printf(message, args...);
			printf("\n");
		}
	}

	template<typename... Args>
	static void ifInfo(const char* message, Args...args)
	{
		if (prior <= Info)
		{
			printf("[Info]");
			printf(message, args...);
			printf("\n");
		}
	}

	template<typename... Args>
	static void ifWarn(const char* message, Args...args)
	{
		if (prior <= Warn)
		{
			printf("[Warn]");
			printf(message, args...);
			printf("\n");
		}
	}

	template<typename... Args>
	static void ifError(const char* message, Args...args)
	{
		if (prior <= Error)
		{
			printf("[Error]");
			printf(message, args...);
			printf("\n");
		}
	}

	template<typename... Args>
	static void ifCritical(const char* message, Args...args)
	{
		if (prior <= Critical)
		{
			printf("[Critical]");
			printf(message, args...);
			printf("\n");
		}
	}
};

LogPriority Logger::prior = Info;
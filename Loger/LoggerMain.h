#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <mutex>
#include <iostream>
#include <regex>

enum LogPriority
{
	Trace, Debug, Info, Warning, Error,
};

class Logger
{
private:
	static LogPriority prior;
	static std::mutex logMute;
	static const char* filepath;
	static FILE* file;

	template<typename... Args>
	static void logOutput(bool NewinFile, bool NewinConsole, LogPriority priority, const char* message_priority_str, const char* message, Args...args)
	{
		if (NewinFile)
		{
			enableFileOutput();
			if (file)
			{
				time_t now = time(0);
				struct tm * timeinfo = localtime(&now);
				char dataTime [20];
				strftime (dataTime, sizeof(dataTime), "%Y-%m-%d %H:%M:%S", timeinfo);
				
				fprintf(file, "%s | %s -> ", dataTime, message_priority_str);
				fprintf(file, message, args...);
				fprintf(file, "\n");
			}
		}
		if (NewinConsole)
		{
			if (prior <= priority)
			{
				std::lock_guard <std::mutex> lock(logMute);
				
				time_t now = time(0);
				struct tm * timeinfo = localtime(&now);
				char dataTime [20];
				strftime (dataTime, sizeof(dataTime), "%Y-%m-%d %H:%M:%S", timeinfo);

				printf("%s | %s -> ", dataTime, message_priority_str);
				printf(message, args...);
				printf("\n");
			}
		}
	}

	static void enableFileOutput()
	{
		if(file != 0)
		{
			fclose(file);
		}

		file = fopen(filepath, "a");

		if (file == 0)
		{
			printf("Faild to open file at %s", filepath);
		}
	}


public:
	static void setPriority(LogPriority newPriority)
	{
		prior = newPriority;
	}

	template<typename... Args>
	static void ifTrace(bool inFile, bool inConsole, const char* message, Args...args)
	{
		logOutput(inFile, inConsole, Trace, "TRACE", message, args...);
	}

	template<typename... Args>
	static void ifDebug(bool inFile, bool inConsole, const char* message, Args...args)
	{
		logOutput(inFile, inConsole, Trace, "DEBUG", message, args...);
	}

	template<typename... Args>
	static void ifInfo(bool inFile, bool inConsole, const char* message, Args...args)
	{	
		logOutput(inFile, inConsole, Trace, "INFO", message, args...);
	}

	template<typename... Args>
	static void ifWarning(bool inFile, bool inConsole, const char* message, Args...args)
	{
		logOutput(inFile, inConsole, Trace, "WARNING", message, args...);
	}

	template<typename... Args>
	static void ifError(bool inFile, bool inConsole, const char* message, Args...args)
	{
		logOutput(inFile, inConsole, Trace, "ERROR", message, args...);
	}

	static void fileName()
	{
		time_t now = time(0);
		struct tm * timeinfo = localtime(&now);
		char dataTime[20];
		strftime (dataTime, sizeof(dataTime), "%Y-%m-%d_%H-%M-%S", timeinfo);	
		

		std::string progName = __FILE__;
    	std::smatch match;
		regex_search(progName, match, std::regex(R"([ \w-]+?(?=\.))"));
		std::string file = match.str();
		const char* fname = file.c_str();

		char* res = new char[strlen(fname) + strlen(dataTime) + 9];
		strcpy(res, fname);
		strcat(res, "_");
		strcat(res, dataTime);
		strcat(res, ".log");
		filepath = res;
	}
	static void fileName(std::string newFilepath)
	{

		time_t now = time(0);
		struct tm * timeinfo = localtime(&now);
		char dataTime[20];
		strftime (dataTime, sizeof(dataTime), "%Y-%m-%d_%H-%M-%S", timeinfo);	

		const char* fname = newFilepath.c_str();

		char* res = new char[strlen(fname) + strlen(dataTime) + 9];
		strcpy(res, fname);
		strcat(res, "_");
		strcat(res, dataTime);
		strcat(res, ".log");
		filepath = res;
		
	}

	static void closeFileOutput()
	{
		fclose(file);
		file = 0;
	}

	
	
};



LogPriority Logger::prior = Trace;
std::mutex Logger::logMute;
const char* Logger::filepath = 0;
FILE* Logger::file = 0;
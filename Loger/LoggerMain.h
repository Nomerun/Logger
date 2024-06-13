#pragma once                                        
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <mutex>
#include <iostream>
#include <regex>

enum LogPriority
{
	Trace, Debug, Info, Warning, Error
};

class Logger
{
private:
	LogPriority prior = Trace;
	std::mutex logMute;
	const char* filepath = 0;
	FILE* file = 0;
	bool NewinFile = false;
	bool NewinConsole = false;

	template<typename... Args>
	void logOutput(const char* fname, int line, LogPriority priority, const char* message_priority_str, const char* message, Args...args)
	{
		if (get_instance().NewinFile)
		{
			if (file)
			{
				time_t now = time(0);
				struct tm * timeinfo = localtime(&now);
				char dataTime [20];
				strftime (dataTime, sizeof(dataTime), "%Y-%m-%d %H:%M:%S", timeinfo);
				
				fprintf(file, "%s | %s | %s%s%d -> ", dataTime, message_priority_str, fname, ":",  line);
				fprintf(file, message, args...);
				fprintf(file, "\n");
			}
		}
		if (get_instance().NewinConsole)
		{
			if (prior <= priority)
			{
				std::lock_guard <std::mutex> lock(logMute);
				
				time_t now = time(0);
				struct tm * timeinfo = localtime(&now);
				char dataTime [20];
				strftime (dataTime, sizeof(dataTime), "%Y-%m-%d %H:%M:%S", timeinfo);

				printf("%s | %s | %s%s%d -> ", dataTime, message_priority_str, fname, ":", line);
				printf(message, args...);
				printf("\n");
			}
		}
	}
	void enableFileOutput()
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



	Logger()
	{

	}

	Logger(const Logger&) = delete;
	Logger& operator = (const Logger&) = delete;

	~Logger()
	{
		if (file != 0)
			fclose(file);
		file = 0;
	}

	static Logger& get_instance()
	{
		static Logger logger;
		return logger;
	}

	//static char createLogOutput () 


public:
	static void setPriority(LogPriority newPriority)
	{
		get_instance().prior = newPriority;
	}

	template<typename... Args>
	static void ifTrace(const char* file, int line, const char* message, Args...args)
	{
		get_instance().logOutput(file, line, Trace, "TRACE", message, args...);
	}

	template<typename... Args>
	static void ifDebug(const char* file, int line, const char* message, Args...args)
	{
		get_instance().logOutput(file, line, Trace, "DEBUG", message, args...);
	}

	template<typename... Args>
	static void ifInfo(const char* file, int line, const char* message, Args...args)
	{
		get_instance().logOutput(file, line, Trace, "INFO", message, args...);
	}

	template<typename... Args>
	static void ifWarning(const char* file, int line, const char* message, Args...args)
	{
		get_instance().logOutput(file, line, Trace, "WARNING", message, args...);
	}

	template<typename... Args>
	static void ifError(const char* file, int line, const char* message, Args...args)
	{
		get_instance().logOutput(file, line, Trace, "ERROR", message, args...);
	}

	static void enableFile(const char* file)
	{
		Logger& logger_instance = get_instance();

		time_t now = time(0);
		struct tm * timeinfo = localtime(&now);
		char dataTime[20];
		strftime (dataTime, sizeof(dataTime), "%Y-%m-%d_%H-%M-%S", timeinfo);	

		std::string progName = file;
    	std::smatch match;
		regex_search(progName, match, std::regex(R"([ \w-]+?(?=\.))"));
		progName = match.str();
		const char* fname = progName.c_str();

		char* res = new char[strlen(fname) + strlen(dataTime) + 9];
		strcpy(res, fname);
		strcat(res, "_");
		strcat(res, dataTime);
		strcat(res, ".log");
		get_instance().filepath = res;
		logger_instance.enableFileOutput();
		get_instance().NewinFile = true;
	}

	static void enableFile(std::string newFilepath, const char* file)
	{
		Logger& logger_instance = get_instance();

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
		get_instance().filepath = res;
		logger_instance.enableFileOutput();
		get_instance().NewinFile = true;
	}

	static void enableConsole()
	{
		get_instance().NewinConsole = true;
	}

	
};

#define LOG_TRACE(message) Logger::ifTrace(__FILE__, __LINE__, "%s", message)
#define LOG_DEBUG(message) Logger::ifDebug(__FILE__, __LINE__, "%s", message)
#define LOG_INFO(message) Logger::ifInfo(__FILE__, __LINE__, "%s", message)
#define LOG_Warning(message) Logger::ifWarning(__FILE__, __LINE__, "%s", message)
#define LOG_ERROR(message) Logger::ifError(__FILE__, __LINE__, "%s", message)
#define LOG_CONSOLE() Logger::enableConsole()
#define LOG_FILE_DEFAULT() Logger::enableFile(__FILE__)
#define LOG_FILE_CUSTOM(name) Logger::enableFile(name, __FILE__)

#pragma once                                        
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <mutex>
#include <regex>

enum LogPriority
{
	Trace, Debug, Info, Warning, Error
};

struct LogFile {
	std::string filename;
	std::ofstream file;
	bool operator==(const LogFile & rhs)
	{
		return(filename == rhs.filename);
	}

	LogFile(const std::string& filename) : filename(filename), file(filename, std::ios::app) {}
};

class Logger
{
private:
	LogPriority prior = Trace;
	std::mutex logMute;
	const char* filepath = 0;
	FILE* file = 0;
	std::vector<LogFile> logFiles;
	bool NewinFile = false;
	bool NewinConsole = false;
	std::thread threads[10];
	int num = 1;

	template<typename... Args>
	void logOutput(const char* fname, int line, LogPriority priority, const char* message_priority_str, const char* message, Args...args)
	{
		if (prior <= priority)
		{
			va_list ar;
			time_t now = time(0);
			struct tm* timeinfo = localtime(&now);
			char dataTime[20];
			strftime(dataTime, sizeof(dataTime), "%Y-%m-%d %H:%M:%S", timeinfo);
			if (get_instance().NewinFile)
			{
				std::lock_guard<std::mutex> lock(logMute);
				for (auto& logFile : logFiles) {
					if (logFile.file.is_open()) {
						char* str = new char[1000];
						sprintf(str, message, args...);
						logFile.file << dataTime << " | " << message_priority_str << " | " << fname << ":" << line << " -> " << str << std::endl;
						delete[] str;
					}
				}
			}
			if (get_instance().NewinConsole)
			{
				std::lock_guard <std::mutex> lock(logMute);
				printf("%s | %s | %s%s%d -> ", dataTime, message_priority_str, fname, ":", line);
				printf(message, args...);
				printf("\n");
			}
		}
	}
	void enableFileOutput(char fname[])
	{
		std::lock_guard<std::mutex> lock(logMute);
		
		logFiles.push_back(LogFile(fname));
		if (!logFiles.back().file.is_open()) {
			std::cerr << "Error opening log file: " << fname << std::endl;
			logFiles.pop_back();
		}
	}

	Logger() {}

	Logger(const Logger&) = delete;
	Logger& operator = (const Logger&) = delete;

	~Logger()
	{
		for (auto& logFile : logFiles) 
		{
			if (logFile.file.is_open()) 
			{
				logFile.file.close();
			}
		}
	}

	static Logger& get_instance()
	{
		static Logger logger;
		return logger;
	}



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
		//get_instance().filepath = res;
		logger_instance.enableFileOutput(res);
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
		//get_instance().filepath = res;
		logger_instance.enableFileOutput(res);
		get_instance().NewinFile = true;
	}

	static void enableConsole()
	{
		get_instance().NewinConsole = true;
	}

	static void CloseFiles()
	{
		for (auto& logFile : get_instance().logFiles)
		{
			if (logFile.file.is_open())
			{
				logFile.file.close();
			}
		}
		get_instance().logFiles.clear();
	}

	static void CloseFile(const char* fname)
	{
		for (auto& logFile : get_instance().logFiles)
		{
			std::smatch match;
			regex_search(logFile.filename, match, std::regex(R"(^[^_]*_*(?=_))"));
			if (match.str() == fname)
			{
				logFile.file.close();
				auto it = std::find(get_instance().logFiles.begin(), get_instance().logFiles.end(), logFile);
				get_instance().logFiles.erase(it);
			}
		}
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
#define LOG_CLOSE_ALL() Logger::CloseFiles()
#define LOG_CLOSE_DEFAULT() Logger::CloseFile(__FILE__)
#define LOG_CLOSE_CUSTOM(name) Logger::CloseFile(name)


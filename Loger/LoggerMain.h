#pragma once                                        
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <mutex>
#include <regex>

/**
* @brief Перечисление приоритетов для лога
*/
enum LogPriority
{
	Trace, Debug, Info, Warning, Error
};
/**
* @brief Структура данных файлов для записи
*/
struct LogFile {
	/* Название файла */
	std::string filename;
	/* Файл */
	std::ofstream file;
	/* Перегрузка булевого оператора сравнения */
	bool operator==(const LogFile & rhs)
	{
		return(filename == rhs.filename);
	}

	LogFile(const std::string& filename) : filename(filename), file(filename, std::ios::app) {}
};

/**
* @brief Базовый класс логгера
* @author Боровков Павел
* @author Аветисян Нажевин
* @version 0.9
*/
class Logger
{
private:
	/* Уровень приоритета */
	LogPriority prior = Trace;
	/* Объект класса Mutex */
	std::mutex logMute;
	/* Вектор для файлов */
	std::vector<LogFile> logFiles;
	/* Булевая переменная для записи в файл */
	bool NewinFile = false;
	/* Булевая переменная для записи в консоль */
	bool NewinConsole = false;
	/* Стандартное имя файла */
	const char* basicName = "LOG";
	
	/**
	* @brief Функция для вывода информации в лог
	* @param file Полный путь к исполняемому файлу
	* @param line Строка кода, в которой выполняется метод
	* @param priority Параметр приоритета
	* @param message_priority_str Название приоритета
	* @param message Формат строки лог
	* @param args Аргументы для записи
	*/
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

	/**
	* @brief Функция для добавления файла в вектор
	* @param fname Полное название файла
	*/
	void enableFileOutput(char fname[])
	{
		std::lock_guard<std::mutex> lock(logMute);
		
		logFiles.push_back(LogFile(fname));
		if (!logFiles.back().file.is_open()) {
			std::cerr << "Error opening log file: " << fname << std::endl;
			logFiles.pop_back();
		}
		else
			get_instance().NewinFile = true;
	}

	/**
	* @brief Конструктор класса
	*/
	Logger() {}

	/**
	* @brief Конструктор класса с удалением копии
	* @param Logger& константная ссылка на объект Logger
	*/
	Logger(const Logger&) = delete;

	/**
	* @brief Перегрузка оператора = с удалением копии
	* @param Logger& константная ссылка на объект Logger
	*/
	Logger& operator = (const Logger&) = delete;

	/**
	* @brief Деконструктор класса с очистой вектора файлов
	*/
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
	/**
	* @brief Механизм для создания единственного экземпляра класса Logger и обеспечения доступа к нему через статический метод
	*/
	static Logger& get_instance()
	{
		static Logger logger;
		return logger;
	}



public:
	/**
	* @brief Метод для установки уровня приоритета в логере
	* @param newPriority Параметр приоритета
	*/
	static void setPriority(LogPriority newPriority)
	{
		get_instance().prior = newPriority;
	}
	/**
	* @brief Метод для записи лога с приоритетом Trace
	* @param file Полный путь к исполняемому файлу
	* @param line Строка кода, в которой выполняется метод
	* @param message Формат строки лог. Для добавления возможности ввода нескольких аргументов используйте формат "%d %s..."
	* @param args Аргументы для записи
	*/
	template<typename... Args>
	static void ifTrace(const char* file, int line, const char* message, Args...args)
	{
		get_instance().logOutput(file, line, Trace, "TRACE", message, args...);
	}

	/**
	* @brief Метод для записи лога с приоритетом Debug
	* @param file Полный путь исполняемого файла
	* @param line Строка кода, в которой выполняется метод
	* @param message Формат строки лог. Для добавления возможности ввода нескольких аргументов используйте формат "%d %s..."
	* @param args Аргументы для записи
	*/
	template<typename... Args>	
	static void ifDebug(const char* file, int line, const char* message, Args...args)
	{
		get_instance().logOutput(file, line, Trace, "DEBUG", message, args...);
	}
	/**
	* @brief Метод для записи лога с приоритетом Info
	* @param file Полный путь исполняемого файла
	* @param line Строка кода, в которой выполняется метод
	*@param message Формат строки лог. Для добавления возможности ввода нескольких аргументов используйте формат "%d %s..."
	* @param args Аргументы для записи
	*/
	template<typename... Args>
	static void ifInfo(const char* file, int line, const char* message, Args...args)
	{
		get_instance().logOutput(file, line, Trace, "INFO", message, args...);
	}

	/**
	* @brief Метод для записи лога с приоритетом Warning
	* @param file Полный путь к исполняемому файлу
	* @param line Строка кода, в которой выполняется метод
	* @param message Формат строки лог. Для добавления возможности ввода нескольких аргументов используйте формат "%d %s..."
	* @param args Аргументы для записи
	*/
	template<typename... Args>
	static void ifWarning(const char* file, int line, const char* message, Args...args)
	{
		get_instance().logOutput(file, line, Trace, "WARNING", message, args...);
	}

	/**
	* @brief Метод для записи лога с приоритетом Error
	* @param file Полный путь к исполняемому файлу
	* @param line Строка кода, в которой выполняется метод
	* @param message Формат строки лог. Для добавления возможности ввода нескольких аргументов используйте формат "%d %s..."
	* @param args Аргументы для записи
	*/
	template<typename... Args>
	static void ifError(const char* file, int line, const char* message, Args...args)
	{
		get_instance().logOutput(file, line, Trace, "ERROR", message, args...);
	}
	
	/**
	* @brief Метод для добавления стандартного имени файла
	*/
	static void enableFile()
	{
		Logger& logger_instance = get_instance();

		time_t now = time(0);
		struct tm * timeinfo = localtime(&now);
		char dataTime[20];
		strftime(dataTime, sizeof(dataTime), "%Y-%m-%d_%H-%M-%S", timeinfo);	

		char* res = new char[strlen(get_instance().basicName) + strlen(dataTime) + 9];
		strcpy(res, get_instance().basicName);
		strcat(res, "_");
		strcat(res, dataTime);
		strcat(res, ".log");
		logger_instance.enableFileOutput(res);
	}

	/**
	* @brief Метод для добавления уникального имени файла
	* @param newFilepath Имя пользовательского файла
	*/
	static void enableFile(std::string newFilepath)
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
		logger_instance.enableFileOutput(res);
	}

	/**
	* @brief Метод для объявления записи логов в консоль
	*/
	static void enableConsole()
	{
		get_instance().NewinConsole = true;
	}

	/**
	* @brief Метод для прекращения записи логов во все файлы
	*/
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
		get_instance().NewinFile = false;
	}

	/**
	* @brief Метод для прекращения записи логов в стандартный файл
	*/
	static void CloseFile()
	{
		for (auto& logFile : get_instance().logFiles)
		{
			std::smatch match;
			regex_search(logFile.filename, match, std::regex(R"(^[^_]*_*(?=_))"));
			if (match.str() == get_instance().basicName)
			{
				logFile.file.close();
				auto it = std::find(get_instance().logFiles.begin(), get_instance().logFiles.end(), logFile);
				get_instance().logFiles.erase(it);
			}
		}
		if (get_instance().logFiles.empty())
			get_instance().NewinFile = false;
	}

	/**
	* @brief Метод для прекращения записи логов в пользовательский файл
	* @param fname Имя пользовательского файла
	*/
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
		if (get_instance().logFiles.empty())
			get_instance().NewinFile = false;
	}
};

/**
* @brief Макрос для записи строки в лог с приоритетом Trace
* @param message Строка для записи в лог
* Для записи нескольких аргументов используйте метод Logger::ifTrace
*/
#define LOG_TRACE(message) Logger::ifTrace(__FILE__, __LINE__, "%s", message)
/**
* @brief Макрос для записи строки в лог с приоритетом Debug
* @param message Строка для записи в лог
* Для записи нескольких аргументов используйте метод Logger::ifDebug
*/
#define LOG_DEBUG(message) Logger::ifDebug(__FILE__, __LINE__, "%s", message)
/**
* @brief Макрос для записи строки в лог с приоритетом Info
* @param message Строка для записи в лог
* Для записи нескольких аргументов используйте метод Logger::ifInfo
*/
#define LOG_INFO(message) Logger::ifInfo(__FILE__, __LINE__, "%s", message)
/**
* @brief Макрос для записи строки в лог с приоритетом Warning
* @param message Строка для записи в лог
* Для записи нескольких аргументов используйте метод Logger::ifWarning
*/
#define LOG_Warning(message) Logger::ifWarning(__FILE__, __LINE__, "%s", message)
/**
* @brief Макрос для записи строки в лог с приоритетом Error
* @param message Строка для записи в лог
* Для записи нескольких аргументов используйте метод Logger::ifError
*/
#define LOG_ERROR(message) Logger::ifError(__FILE__, __LINE__, "%s", message)
/**
* @brief Макрос для объявления записи логов в консоль
*/
#define LOG_CONSOLE() Logger::enableConsole()
/**
* @brief Макрос для создания стандартного файла и записи в него лога
*/
#define LOG_FILE_DEFAULT() Logger::enableFile()
/**
* @brief Макрос для создания пользовател скогофайла и записи в него лога
* @@param name мя для пользовательского файла
*/
#define LOG_FILE_CUSTOM(name) Logger::enableFile(name)
/**
* @brief Макрос для прекращения записи логов во все файлы
*/
#define LOG_CLOSE_ALL() Logger::CloseFiles()
/**
* @brief Метод для прекращения записи логов в стандартный файл
*/
#define LOG_CLOSE_DEFAULT() Logger::CloseFile()
/**
* @brief Метод для прекращения записи логов в пользовательский файл
* @param name Имя для пользовательского файла
*/
#define LOG_CLOSE_CUSTOM(name) Logger::CloseFile(name)


#include "misc/Logger.hpp"

#include <ctime>
#include <iomanip>

namespace RIS
{
    Logger *Logger::instance = nullptr;

    Logger& Logger::Instance()
    {
        if(!instance)
            instance = new Logger();
        return *instance;
    }

    void Logger::Destroy()
    {
        if(instance)
        {
            delete instance;
            instance = nullptr;
        }
    }

    Logger::Logger()
        : logFile("log.txt")
    {

    }

    Logger::~Logger()
    {
        logFile.close();
    }

    void Logger::PutTime(std::ofstream &stream)
	{
		time_t time = std::time(nullptr);
		struct tm timeinfo;
#ifdef _WIN32
		localtime_s(&timeinfo, &time);
#else
        localtime_r(&time, &timeinfo);
#endif
		stream << "[" << std::put_time(&timeinfo, "%d-%m-%Y %H-%M-%S") << "]";
	}
}

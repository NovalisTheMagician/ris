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
		localtime_s(&timeinfo, &time);
		stream << "[" << std::put_time(&timeinfo, "%d-%m-%Y %H-%M-%S") << "]";
	}
}

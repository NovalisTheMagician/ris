#pragma once

#include <string>
#include <fstream>

namespace RIS
{
    class Logger
    {
    public:
        static Logger& Instance();
        static void Destroy();

    public:
        Logger();
        ~Logger();

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger &) = delete;

        Logger(Logger&&) = delete;
        Logger& operator=(Logger&&) = delete;

        template<typename T>
        void Warning(const T &v);
        template<typename T>
        void Info(const T &text);
        template<typename T>
        void Error(const T &text);

    private:
        void PutTime(std::ofstream &stream);

    private:
        static Logger *instance;

    private:
        std::ofstream logFile;

    };

    template<typename T>
    void Logger::Warning(const T &v)
    {
        PutTime(logFile);
        logFile << "[WARN]" << v << std::endl;
    }

    template<typename T>
    void Logger::Info(const T &v)
    {
        PutTime(logFile);
        logFile << "[INFO]" << v << std::endl;
    }

    template<typename T>
    void Logger::Error(const T &v)
    {
        PutTime(logFile);
        logFile << "[ERRO]" << v << std::endl;
    }
}

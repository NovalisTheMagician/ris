#pragma once

#include <string>

#include <functional>

namespace RIS
{
    using ConsoleFunc = std::function<void(std::string, std::string, std::string)>;

    class IConsole
    {
    public:
        virtual ~IConsole() = default;

        virtual void Open() = 0;
        virtual void Close() = 0;

        virtual void Print(const std::string &msg) = 0;

        virtual void BindVar(const std::string &name, const long *var) = 0;
        virtual void BindVar(const std::string &name, const std::string *var) = 0;

        virtual void BindFunc(const std::string &name, ConsoleFunc func) = 0;

    };

    class IUserinterface
    {
    public:
        virtual ~IUserinterface() = default;

        virtual void InitializeRootElements() = 0;

        virtual void LoadLayout(const std::string &layout) = 0;

        virtual IConsole& GetConsole() = 0;

        virtual void Draw() = 0;
        virtual void Update() = 0;
    };
}

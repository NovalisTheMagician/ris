#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <functional>

#include "common/Timer.hpp"

namespace RIS
{
    using ConsoleFunc = std::function<std::string(std::vector<std::string>)>;

    namespace Helpers
    {
        inline std::function<std::string(std::vector<std::string>)> BoolFunc(bool &value, const std::string &onMsg, const std::string &offMsg)
        {
            return [&, onMsg, offMsg](auto& params)
            {
                using namespace std::literals;
                if(params.size() == 0)
                {
                    return std::to_string(value);
                }
                else
                {
                    try
                    {
                        value = std::stoi(params.at(0));
                        return value ? onMsg : offMsg;
                    }
                    catch(const std::exception& e)
                    {
                        return "Invalid Value"s;
                    }
                }
            };
        }
    }

    class IConsole
    {
    public:
        virtual ~IConsole() = default;

        virtual void Open() = 0;
        virtual void Close() = 0;
        virtual void Toggle() = 0;

        virtual void Print(const std::string &msg) = 0;

        virtual void BindFunc(const std::string &name, ConsoleFunc func) = 0;

    };

    class IUserinterface
    {
    public:
        virtual ~IUserinterface() = default;

        virtual void InitializeRootElements() = 0;

        virtual void LoadLayout(const std::string &layout) = 0;

        virtual IConsole& GetConsole() = 0;

        virtual void RegisterFunctions() = 0;

        virtual void Draw() = 0;
        virtual void Update(const Timer &timer) = 0;
    };
}

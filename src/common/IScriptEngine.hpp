#pragma once

#include <string>

namespace RIS
{
    class IScriptEngine
    {
    public:
        virtual ~IScriptEngine() = default;

        virtual void LoadScript(const std::string &scriptName, const std::string &module) = 0;

        

    };
}

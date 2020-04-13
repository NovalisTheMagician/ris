#pragma once

#include "IScriptEngine.hpp"

#include "SystemLocator.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "IUserinterface.hpp"

#include <LuaState.h>
#include <LuaPrimitives.h>

#include <string>
#include <vector>
#include <stdexcept>

namespace RIS
{
    struct LuaException : public std::runtime_error
    {
        LuaException(std::string reason) : std::runtime_error(reason.c_str()) {}
    };

    using LTable = lua::Table;
    using LValue = lua::Value;
    using LState = lua::State;

    class LuaScriptEngine : public IScriptEngine
    {
    public:
        LuaScriptEngine(const SystemLocator &systems, Config &config);
        ~LuaScriptEngine();

        void LoadScript(const std::string &scriptName, const std::string &module = "") override;

        void PostInit() override;
        void Reload(const std::string &script);

        LState& GetState();

        template<typename T>
        void Register(T fun, const std::string &name);
        template<typename T>
        void Register(T fun, const std::string &table, const std::string &name);

        template<typename Ret, typename... Args>
        Ret CallFunction(const std::string &table, const std::string &name, Args...);
        template<typename... Args>
        void CallFunction(const std::string &table, const std::string &name, Args...);

    private:
        const SystemLocator &systems;
        Config &config;

        std::vector<std::string> loadedScripts;

        lua::State luaState;

    };

    template<typename T>
    void LuaScriptEngine::Register(T fun, const std::string &table, const std::string &name)
    {
        if(!luaState[table.c_str()])
            luaState.set(table.c_str(), lua::Table());
        luaState[table.c_str()].set(name.c_str(), fun);
    }

    template<typename T>
    void LuaScriptEngine::Register(T fun, const std::string &name)
    {
        luaState.set(name.c_str(), fun);
    }

    template<typename Ret, typename... Args>
    Ret LuaScriptEngine::CallFunction(const std::string &table, const std::string &name, Args... args)
    {
        lua::Value luaVal;
        if(!table.empty())
            luaVal = luaState[table.c_str()][name.c_str()];
        else
            luaVal = luaState[name.c_str()];
        if(!luaVal.is<lua::Callable>())
        {
            std::string msg = "function \"" + table + "." + name + "\" not found";
            systems.GetUserinterface().GetConsole().Print(msg);
            Logger::Instance().Error(msg);
        }
        
        try
        {
            return luaVal.call(args...);
        }
        catch(const std::exception &e)
        {
            systems.GetUserinterface().GetConsole().Print(e.what());
            Logger::Instance().Error(e.what());
        }
    }

    template<typename... Args>
    void LuaScriptEngine::CallFunction(const std::string &table, const std::string &name, Args... args)
    {
        lua::Value luaVal;
        if(!table.empty())
            luaVal = luaState[table.c_str()][name.c_str()];
        else
            luaVal = luaState[name.c_str()];
        if(!luaVal.is<lua::Callable>())
        {
            std::string msg = "function \"" + table + "." + name + "\" not found";
            systems.GetUserinterface().GetConsole().Print(msg);
            Logger::Instance().Error(msg);
        }

        try
        {
            luaVal.call(args...);
        }
        catch(const std::exception &e)
        {
            systems.GetUserinterface().GetConsole().Print(e.what());
            Logger::Instance().Error(e.what());
        }
    }
}

#pragma once

#include "IScriptEngine.hpp"

#include "SystemLocator.hpp"
#include "Config.hpp"

#include <LuaState.h>
#include <LuaPrimitives.h>

#include <string>
#include <stdexcept>

namespace RIS
{
    struct LuaException : public std::runtime_error
    {
        LuaException(std::string reason) : std::runtime_error(reason.c_str()) {}
    };

    class LuaScriptEngine : public IScriptEngine
    {
    public:
        LuaScriptEngine(const SystemLocator &systems, Config &config);
        ~LuaScriptEngine();

        void LoadScript(const std::string &scriptName, const std::string &module = "") override;

        void PostInit() override;

        template<typename T>
        void RegisterFunction(T fun, const std::string &name);

        template<typename T>
        void RegisterFunction(T fun, const std::string &table, const std::string &name);

        template<typename Ret, typename... Args>
        Ret CallFunction(const std::string &table, const std::string &name, Args...);

        template<typename... Args>
        void CallFunction(const std::string &table, const std::string &name, Args...);

    private:
        const SystemLocator &systems;
        Config &config;

        lua::State luaState;

    };

    template<typename T>
    void LuaScriptEngine::RegisterFunction(T fun, const std::string &table, const std::string &name)
    {
        if(!luaState[table.c_str()])
            luaState.set(table.c_str(), lua::Table());
        luaState[table.c_str()].set(name.c_str(), fun);
    }

    template<typename T>
    void LuaScriptEngine::RegisterFunction(T fun, const std::string &name)
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
        if(!luaVal.is<lua::Callable>()) throw LuaException("function not found");
        return luaVal(args...);
    }

    template<typename... Args>
    void LuaScriptEngine::CallFunction(const std::string &table, const std::string &name, Args... args)
    {
        lua::Value luaVal;
        if(!table.empty())
            luaVal = luaState[table.c_str()][name.c_str()];
        else
            luaVal = luaState[name.c_str()];
        if(!luaVal.is<lua::Callable>()) throw LuaException("function not found");
        luaVal(args...);
    }
}

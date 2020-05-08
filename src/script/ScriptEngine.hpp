#pragma once

#include "misc/Logger.hpp"
#include "ui/Userinterface.hpp"

//#include "RIS.hpp"

#include <LuaState.h>
#include <LuaPrimitives.h>

#include <string>
#include <vector>
#include <stdexcept>

namespace RIS
{
    namespace Script
    {
        struct ScriptException : public std::runtime_error
        {
            ScriptException(std::string reason) : std::runtime_error(reason.c_str()) {}
        };

        using LTable = lua::Table;
        using LValue = lua::Value;
        using LState = lua::State;

        class ScriptEngine
        {
        public:
            ScriptEngine() = default;
            ~ScriptEngine() = default;
            ScriptEngine(const ScriptEngine&) = delete;
            ScriptEngine& operator=(const ScriptEngine&) = delete;
            ScriptEngine(ScriptEngine&&) = default;
            ScriptEngine& operator=(ScriptEngine&&) = default;

            void LoadScript(const std::string &scriptName, const std::string &module = "");

            void PostInit();
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
            std::vector<std::string> loadedScripts;

            lua::State luaState;

        };

        template<typename T>
        void ScriptEngine::Register(T fun, const std::string &table, const std::string &name)
        {
            if(!luaState[table.c_str()])
                luaState.set(table.c_str(), lua::Table());
            luaState[table.c_str()].set(name.c_str(), fun);
        }

        template<typename T>
        void ScriptEngine::Register(T fun, const std::string &name)
        {
            luaState.set(name.c_str(), fun);
        }

        template<typename Ret, typename... Args>
        Ret ScriptEngine::CallFunction(const std::string &table, const std::string &name, Args... args)
        {
            lua::Value luaVal;
            if(!table.empty())
                luaVal = luaState[table.c_str()][name.c_str()];
            else
                luaVal = luaState[name.c_str()];
            if(!luaVal.is<lua::Callable>())
            {
                std::string msg = "function \"" + table + "." + name + "\" not found";
                //GetSystems().GetUserinterface().GetConsole().Print(msg);
                Logger::Instance().Error(msg);
            }
            
            try
            {
                return luaVal.call(args...);
            }
            catch(const std::exception &e)
            {
                //GetSystems().GetUserinterface().GetConsole().Print(e.what());
                Logger::Instance().Error(e.what());
            }
        }

        template<typename... Args>
        void ScriptEngine::CallFunction(const std::string &table, const std::string &name, Args... args)
        {
            lua::Value luaVal;
            if(!table.empty())
                luaVal = luaState[table.c_str()][name.c_str()];
            else
                luaVal = luaState[name.c_str()];
            if(!luaVal.is<lua::Callable>())
            {
                std::string msg = "function \"" + table + "." + name + "\" not found";
                //GetSystems().GetUserinterface().GetConsole().Print(msg);
                Logger::Instance().Error(msg);
            }

            try
            {
                luaVal.call(args...);
            }
            catch(const std::exception &e)
            {
                //GetSystems().GetUserinterface().GetConsole().Print(e.what());
                Logger::Instance().Error(e.what());
            }
        }
    }
}

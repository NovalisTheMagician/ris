#pragma once

#include "misc/Logger.hpp"

#include "RIS.hpp"

#include "ui/Console.hpp"

#include "RisExcept.hpp"

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include <squall/squall_vmstd.hpp>

namespace RIS
{
    namespace Script
    {
        struct ScriptException : public RISException
        {
            ScriptException(const std::string &reason) : RISException(reason) {}
        };

        class ScriptEngine
        {
        public:
            ScriptEngine();
            ~ScriptEngine() = default;
            ScriptEngine(const ScriptEngine&) = delete;
            ScriptEngine& operator=(const ScriptEngine&) = delete;
            ScriptEngine(ScriptEngine&&) = default;
            ScriptEngine& operator=(ScriptEngine&&) = default;

            void LoadScripts();
            void PostInit();
            void Reload();

            template<typename T>
            void Register(const std::string &name, T func);

            template<typename Ret, typename... Args>
            Ret CallFunction(const std::string &name, Args...);

        private:
            //only a pointer so we can reset the vm when we hotreload scripts
            std::unique_ptr<squall::VMStd> vm;

        };

        template<typename T>
        void ScriptEngine::Register(const std::string &name, T func)
        {
            vm->defun(name, func);
        }

        template<typename Ret, typename... Args>
        Ret ScriptEngine::CallFunction(const std::string &name, Args... args)
        {
            try
            {
                return vm->call<Ret>(name, args...);
            }
            catch(const std::exception& e)
            {
                GetConsole().Print(e.what());
                return Ret();
            }
        }
    }
}

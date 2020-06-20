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
#include <squall/squall_klass.hpp>

namespace RIS
{
    namespace Script
    {
        struct ScriptException : public RISException
        {
            ScriptException(const std::string &reason) : RISException(reason) {}
        };

        template<typename C, typename Base = void>
        class ScriptClass
        {
        public:
            ScriptClass(squall::VMStd *vm, const std::string &name) : vm(vm), klass(*vm, name) {}
            ~ScriptClass() = default;

            ScriptClass(const ScriptClass&) = delete;
            ScriptClass(ScriptClass&&) = delete;
            ScriptClass& operator=(const ScriptClass&) = delete;
            ScriptClass& operator=(ScriptClass&&) = delete;

            template<typename T>
            ScriptClass& RegisterFunc(const std::string &name, T func)
            {
                klass.func(name, func);
                return *this;
            }

        private:
            squall::VMStd *vm;
            squall::Klass<C, Base> klass;

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

            template<typename C, typename Base = void>
            ScriptClass<C, Base> RegisterKlass(const std::string &name);

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

        template<typename C, typename Base>
        ScriptClass<C, Base> ScriptEngine::RegisterKlass(const std::string &name)
        {
            return ScriptClass<C, Base>(vm.get(), name);
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

#pragma once

#include "misc/Logger.hpp"

#include "RIS.hpp"

#include "ui/Console.hpp"

#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <unordered_map>
#include <optional>
#include <utility>
#include <type_traits>

#include <libtcc.h>

namespace RIS
{
    namespace Script
    {
        //https://stackoverflow.com/a/45365798
        template<typename Callable>
        struct storage
        {
            storage() {}
            std::decay_t<Callable> callable;
        };

        template<int, typename Callable, typename Ret, typename... Args>
        auto fnptr_(Callable&& c, Ret (*)(Args...))
        {
            static bool used = false;
            static storage<Callable> s;
            using type = decltype(s.callable);

            if(used)
                s.callable.~type();
            new (&s.callable) type(std::forward<Callable>(c));
            used = true;

            return [](Args... args) -> Ret {
                return Ret(s.callable(std::forward<Args>(args)...));
            };
        }

        template<typename Fn, int N = 0, typename Callable>
        Fn* fnptr(Callable&& c)
        {
            return fnptr_<N>(std::forward<Callable>(c), (Fn*)nullptr);
        }

        struct ScriptException : public std::runtime_error
        {
            ScriptException(std::string reason) : std::runtime_error(reason.c_str()) {}
        };

        class ScriptEngine
        {
        public:
            ScriptEngine();
            ~ScriptEngine();
            ScriptEngine(const ScriptEngine&) = delete;
            ScriptEngine& operator=(const ScriptEngine&) = delete;
            ScriptEngine(ScriptEngine&&) = default;
            ScriptEngine& operator=(ScriptEngine&&) = default;

            void LoadScripts();
            void PostInit();
            void Reload();

            template<typename Func, int Slot = 0>
            void Register(const std::string &name, std::function<Func> func);

            template<typename T>
            std::optional<T> GetSymbol(const std::string &name);

            template<typename Ret, typename... Args>
            Ret CallFunction(const std::string &name, Args...);

        private:
            TCCState* state;

            std::unordered_map<std::string, void*> symbols;

        };

        template<typename Func, int Slot>
        void ScriptEngine::Register(const std::string &name, std::function<Func> func)
        {
            auto fn = fnptr<Func, Slot>(func);
            tcc_add_symbol(state, name.c_str(), fn);
        }

        template<typename T>
        std::optional<T> ScriptEngine::GetSymbol(const std::string &name)
        {
            if(symbols.count(name) > 0)
            {
                T* val = static_cast<T*>(symbols.at(name));
                return *val;
            }
            
            GetConsole().Print("Symbol \"" + name + "\" not found");
            return std::nullopt;
        }

        template<typename Ret, typename... Args>
        Ret ScriptEngine::CallFunction(const std::string &name, Args... args)
        {
            using FuncPtr = Ret(*)(Args...);
            if(symbols.count(name) > 0)
            {
                void *rawFunc = symbols.at(name);
                FuncPtr func = static_cast<FuncPtr>(rawFunc);
                return func(args...);
            }
            else
            {
                GetConsole().Print("Function \"" + name + "\" not found");
                return Ret();
            }
        }
    }
}

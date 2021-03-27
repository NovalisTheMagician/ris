#pragma once

#include "misc/Logger.hpp"

#include "RIS.hpp"

#include "ui/Console.hpp"

#include "RisExcept.hpp"

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <utility>
#include <optional>

#include <sol/sol.hpp>

namespace RIS::Script
{
#if 0
    struct ScriptException : public RISException
    {
        ScriptException(const std::string &reason) : RISException(reason) {}
    };

    template<typename C, typename Base = void>
    class ScriptClass
    {
    public:
        ScriptClass(sol::table &table, const std::string &name) : userType(table.new_usertype<C>(name)) 
        {
            if constexpr (!std::is_void_v<Base>)
            {
                userType[sol::base_classes] = sol::bases<Base>();
            }
        }
        ScriptClass(sol::state &vm, const std::string &name) : userType(vm.new_usertype<C>(name)) 
        {
            if constexpr (!std::is_void_v<Base>)
            {
                userType[sol::base_classes] = sol::bases<Base>();
            }
        }
        ~ScriptClass() = default;

        ScriptClass(const ScriptClass&) = delete;
        ScriptClass(ScriptClass&&) = delete;
        ScriptClass& operator=(const ScriptClass&) = delete;
        ScriptClass& operator=(ScriptClass&&) = delete;

        template<typename... C>
        ScriptClass& Ctors()
        {
            userType.set_function("new", sol::constructors<C...>());
            return *this;
        }

        template<typename... T>
        ScriptClass& FuncOverload(const std::string &name, T... func)
        {
            userType.set_function(name, sol::overload(func...));
            return *this;
        }

        template<typename T>
        ScriptClass& Func(const std::string &name, T func)
        {
            userType.set_function(name, func);
            return *this;
        }

        template<typename T>
        ScriptClass& Var(const std::string &name, T C::* var)
        {
            userType.set(name, var);
            return *this;
        }

    private:
        sol::usertype<C> userType;

    };

    class ScriptNamespace
    {
    public:
        ScriptNamespace(sol::table &parent, const std::string &name) : table(parent[name].get_or_create<sol::table>()) {}
        ScriptNamespace(sol::state &vm, const std::string &name) : table(vm[name].get_or_create<sol::table>()) {}
        ~ScriptNamespace() = default;

        ScriptNamespace(const ScriptNamespace&) = delete;
        ScriptNamespace(ScriptNamespace&&) = delete;
        ScriptNamespace& operator=(const ScriptNamespace&) = delete;
        ScriptNamespace& operator=(ScriptNamespace&&) = delete;

        template<typename T>
        ScriptNamespace& Func(const std::string &name, T func)
        {
            table.set_function(name, func);
            return *this;
        }

        template<typename... T>
        ScriptNamespace& FuncOverload(const std::string &name, T... func)
        {
            table.set_function(name, sol::overload(func...));
            return *this;
        }

        ScriptNamespace Namespace(const std::string &name)
        {
            return ScriptNamespace(table, name);
        }

        template<typename Ret, typename... Args>
        Ret Call(const std::string &name, Args&&... args)
        {
            std::optional<sol::protected_function> opt = table[name];
            if(opt)
            {
                auto fun = opt.value();
                sol::protected_function_result result = fun(std::forward(args)...);
                if(result.valid())
                {
                    if constexpr (!std::is_void_v<Ret>)
                        return result;
                }
                else
                {
                    sol::error err = result;
                    std::string what = err.what();
                    Logger::Instance().Error(what);
                    GetConsole().Print(what);
                }
            }
            return Ret();
        }

        template<typename C, typename B = void>
        ScriptClass<C, B> Class(const std::string &name)
        {
            return ScriptClass<C, B>(table, name);
        }

    private:
        sol::table table;

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
        void Func(const std::string &name, T func)
        {
            vm.set_function(name, func);
        }

        template<typename... T>
        void FuncOverload(const std::string &name, T... func)
        {
            vm.set_function(name, sol::overload(func...));
        }

        ScriptNamespace Namespace(const std::string &name)
        {
            return ScriptNamespace(vm, name);
        }

        template<typename C, typename Base = void>
        ScriptClass<C, Base> Class(const std::string &name)
        {
            return ScriptClass<C, Base>(vm, name);
        }

        template<typename T>
        void Var(const std::string &name, T &&var)
        {
            vm.set(name, std::forward(var));
        }

        template<typename Ret, typename... Args>
        Ret Call(const std::string &name, Args&&... args)
        {
            std::optional<sol::protected_function> opt = vm[name];
            if(opt)
            {
                auto fun = opt.value();
                sol::protected_function_result result = fun(std::forward(args)...);
                if(result.valid())
                {
                    if constexpr (!std::is_void<Ret>::value)
                        return result;
                }
                else
                {
                    sol::error err = result;
                    std::string what = err.what();
                    Logger::Instance().Error(what);
                    GetConsole().Print(what);
                }
            }
            return Ret();
        }

    private:
        sol::state vm;

    };
#endif
}

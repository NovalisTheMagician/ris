#include "script/ScriptEngine.hpp"

#include "ui/Console.hpp"

#include "misc/Logger.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include "RIS.hpp"

using namespace std::literals;

namespace RIS
{
    namespace Script
    {
        void ScriptEngine::LoadScript(const std::string &scriptName, const std::string &module)
        {
            auto &loader = GetSystems().GetLoader();
            try
            {
                std::string scriptContent = *loader.Load<std::string>(scriptName).get();
                luaState.doString(scriptContent);
                loadedScripts.insert(loadedScripts.end(), scriptName);
            }
            catch(const lua::LoadError &e)
            {
                UI::Console &console = GetSystems().GetUserinterface().GetConsole();
                console.Print("Lua load error: "s + e.what());
                Logger::Instance().Error("Lua load error: "s + e.what());
            }
            catch(const std::exception &e)
            {
                UI::Console &console = GetSystems().GetUserinterface().GetConsole();
                console.Print("Script load error: "s + e.what());
                Logger::Instance().Error("Script load error: "s + e.what());
            }
        }

        void ScriptEngine::PostInit()
        {
            UI::Console &console = GetSystems().GetUserinterface().GetConsole();

            auto execFunc = [this](std::vector<std::string> params)
            { 
                if(params.size() == 0) return std::string();
                try
                {
                    std::string script;
                    for (const auto &elem : params) script += elem + " ";
                    auto ret = luaState.doString(script);
                    //if(ret) return ret.toString();
                    return std::string();
                }
                catch(const std::exception &e)
                {
                    return std::string(e.what());
                }
            };

            console.BindFunc("execute", execFunc);
            console.BindFunc("exec", execFunc);
            console.BindFunc("reload", [this](std::vector<std::string> params)
            {
                if(params.size() == 0)
                {
                    Reload("");
                } 
                else
                {
                    std::string script = params.at(0);
                    Reload(script);
                }
                return "";
            });

            auto &loader = GetSystems().GetLoader();
        }

        void ScriptEngine::Reload(const std::string &script)
        {
            if(script.empty())
            {
                std::vector<std::string> scripts = loadedScripts;
                loadedScripts.clear();
                std::for_each(scripts.begin(), scripts.end(), [this](const std::string &script)
                { 
                    LoadScript(script);
                });
            }
            else
            {
                auto elem = std::find(loadedScripts.begin(), loadedScripts.end(), script);
                if(elem != loadedScripts.end())
                    loadedScripts.erase(elem);
                LoadScript(script);
            }
        }

        LState& ScriptEngine::GetState()
        {
            return luaState;
        }
    }
}
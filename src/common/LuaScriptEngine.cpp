#include "LuaScriptEngine.hpp"

#include "IUserinterface.hpp"

#include "Logger.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std::literals;

namespace RIS
{
    LuaScriptEngine::LuaScriptEngine(const SystemLocator &systems, Config &config)
        : systems(systems), config(config)
    {
        
    }

    LuaScriptEngine::~LuaScriptEngine()
    {

    }

    void LuaScriptEngine::LoadScript(const std::string &scriptName, const std::string &module)
    {
        ILoader &loader = systems.GetLoader();

        auto &fut = loader.LoadAsset(AssetType::SCRIPT, scriptName);
        auto [data, size] = fut.get();
        std::string scriptContent(reinterpret_cast<char*>(data.get()), size);

        loadedScripts.insert(loadedScripts.end(), scriptName);
        try
        {
            luaState.doString(scriptContent);
        }
        catch(const lua::LoadError &e)
        {
            IConsole &console = systems.GetUserinterface().GetConsole();
            console.Print("Lua load error: "s + e.what());
            Logger::Instance().Error("Lua load error: "s + e.what());
        }
    }

    void LuaScriptEngine::PostInit()
    {
        IConsole &console = systems.GetUserinterface().GetConsole();

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
    }

    void LuaScriptEngine::Reload(const std::string &script)
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
            loadedScripts.erase(std::find(loadedScripts.begin(), loadedScripts.end(), script));
            LoadScript(script);
        }
        
    }

    LState& LuaScriptEngine::GetState()
    {
        return luaState;
    }
}
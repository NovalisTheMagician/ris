#include "LuaScriptEngine.hpp"

#include "IUserinterface.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

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

        luaState.doString(scriptContent);
    }

    void LuaScriptEngine::PostInit()
    {
        IConsole &console = systems.GetUserinterface().GetConsole();
        console.BindFunc("execute", [this](std::vector<std::string> params)
        { 
            if(params.size() == 0) return std::string();
            try
            {
                std::string script;
                for (const auto &elem : params) script += elem;
                auto ret = luaState.doString(script);
                //if(ret) return ret.toString();
                return std::string();
            }
            catch(const std::exception &e)
            {
                return std::string(e.what());
            }
        });
    }
}
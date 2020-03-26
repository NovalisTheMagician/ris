#include "LuaScriptEngine.hpp"

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
}
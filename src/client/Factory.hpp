#pragma once

#include "common/IWindow.hpp"
#include "common/IRenderer.hpp"
#include "common/INetwork.hpp"
#include "common/IAudio.hpp"
#include "common/IUserinterface.hpp"
#include "common/ILoader.hpp"
#include "common/IInput.hpp"
#include "common/IScriptEngine.hpp"
#include "common/LuaScriptEngine.hpp"

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include "GLFWWindow.hpp"

#include <memory>
#include <string>

#undef CreateWindow

namespace RIS
{
    class SystemFactory
    {
    public:
        SystemFactory(SystemLocator &locator, Config &config);
        ~SystemFactory();

        SystemFactory(const SystemFactory &) = delete;
        SystemFactory& operator= (const SystemFactory &) = delete;

        SystemFactory(SystemFactory &&) = delete;
        SystemFactory& operator= (SystemFactory &&) = delete;

        std::unique_ptr<IWindow> CreateWindow(const std::string &title) const;
        std::unique_ptr<IRenderer> CreateRenderer() const;
        std::unique_ptr<INetwork> CreateNetwork() const;
        std::unique_ptr<IAudio> CreateAudio() const;
        std::unique_ptr<IUserinterface> CreateUserinterface() const;
        std::unique_ptr<ILoader> CreateLoader(const std::string &debugAssetFolder) const;
        std::unique_ptr<IInput> CreateInput() const;
        std::unique_ptr<LuaScriptEngine> CreateScriptEngine() const;

    private:
        SystemLocator &locator;
        Config &config;

    };
}
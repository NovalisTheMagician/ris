#pragma once

#include "SystemLocator.hpp"

#include <memory>
#include <string>

#undef CreateWindow

namespace RIS
{
    class SystemFactory
    {
    public:
        SystemFactory(SystemLocator &locator);
        ~SystemFactory() = default;

        SystemFactory(const SystemFactory &) = delete;
        SystemFactory& operator= (const SystemFactory &) = delete;

        SystemFactory(SystemFactory &&) = delete;
        SystemFactory& operator= (SystemFactory &&) = delete;

        std::unique_ptr<Window::Window> CreateWindow(const std::string &title) const;
        std::unique_ptr<Graphics::Renderer> CreateRenderer() const;
        std::unique_ptr<Audio::AudioEngine> CreateAudio() const;
        std::unique_ptr<UI::Userinterface> CreateUserinterface() const;
        std::unique_ptr<Loader::Loader> CreateLoader(const std::string &debugAssetFolder) const;
        std::unique_ptr<Input::Input> CreateInput() const;
        std::unique_ptr<Script::ScriptEngine> CreateScriptEngine() const;

    private:
        SystemLocator &locator;

    };
}
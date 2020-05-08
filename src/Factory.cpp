#include "Factory.hpp"

using std::string;

namespace RIS
{
    using WindowPtr = std::unique_ptr<Window::Window>;
    using RendererPtr = std::unique_ptr<Graphics::Renderer>;
    using AudioPtr = std::unique_ptr<Audio::AudioEngine>;
    using UserinterfacePtr = std::unique_ptr<UI::Userinterface>;
    using LoaderPtr = std::unique_ptr<Loader::Loader>;
    using InputPtr = std::unique_ptr<Input::Input>;
    using ScriptEnginePtr = std::unique_ptr<Script::ScriptEngine>;

    SystemFactory::SystemFactory(SystemLocator &locator)
        : locator(locator)
    {}

    WindowPtr SystemFactory::CreateWindow(const string &title) const
    {
        auto system = std::make_unique<Window::Window>(title);
        locator.Provide(system.get());
        return system;
    }

    RendererPtr SystemFactory::CreateRenderer() const
    {
        auto system = std::make_unique<Graphics::Renderer>();
        locator.Provide(system.get());
        return system;
    }

    AudioPtr SystemFactory::CreateAudio() const
    {
        auto system = std::make_unique<Audio::AudioEngine>();
        locator.Provide(system.get());
        return system;
    }

    UserinterfacePtr SystemFactory::CreateUserinterface() const
    {
        auto system = std::make_unique<UI::Userinterface>();
        locator.Provide(system.get());
        return system;
    }

    LoaderPtr SystemFactory::CreateLoader(const std::string &debugAssetFolder) const
    {
        auto system = std::make_unique<Loader::Loader>(debugAssetFolder);
        locator.Provide(system.get());
        return system;
    }

    InputPtr SystemFactory::CreateInput() const
    {
        auto &window = locator.GetWindow();

        auto system = std::make_unique<Input::Input>(window);
        locator.Provide(system.get());
        return system;
    }

    ScriptEnginePtr SystemFactory::CreateScriptEngine() const
    {
        auto system = std::make_unique<Script::ScriptEngine>();
        locator.Provide(system.get());
        return system;
    }
}

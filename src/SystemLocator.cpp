#include "SystemLocator.hpp"

namespace RIS
{
    void SystemLocator::Provide(Window::Window *window)
    {
        this->window = window;
    }

    void SystemLocator::Provide(Graphics::Renderer *renderer)
    {
        this->renderer = renderer;
    }

    void SystemLocator::Provide(Audio::AudioEngine *audio)
    {
        this->audio = audio;
    }

    void SystemLocator::Provide(UI::Userinterface *userinterface)
    {
        this->userinterface = userinterface;
    }

    void SystemLocator::Provide(Loader::Loader *loader)
    {
        this->loader = loader;
    }

    void SystemLocator::Provide(Input::Input *input)
    {
        this->input = input;
    }

    void SystemLocator::Provide(Script::ScriptEngine *script)
    {
        this->script = script;
    }

    Window::Window& SystemLocator::GetWindow() const
    {
        if(window == nullptr)
            throw SystemNotProvidedException("Window");

        return *window;
    }

    Graphics::Renderer& SystemLocator::GetRenderer() const
    {
        if(renderer == nullptr)
            throw SystemNotProvidedException("Renderer");
            
        return *renderer;
    }

    Audio::AudioEngine& SystemLocator::GetAudio() const
    {
        if(audio == nullptr)
            throw SystemNotProvidedException("Audio");
            
        return *audio;
    }

    UI::Userinterface& SystemLocator::GetUserinterface() const
    {
        if(userinterface == nullptr)
            throw SystemNotProvidedException("Userinterface");
            
        return *userinterface;
    }

    Loader::Loader& SystemLocator::GetLoader() const
    {
        if(loader == nullptr)
            throw SystemNotProvidedException("Loader");
            
        return *loader;
    }

    Input::Input& SystemLocator::GetInput() const
    {
        if(input == nullptr)
            throw SystemNotProvidedException("Input");
            
        return *input;
    }

    Script::ScriptEngine& SystemLocator::GetScriptEngine() const
    {
        if(script == nullptr)
            throw SystemNotProvidedException("ScriptEngine");
            
        return *script;
    }
}

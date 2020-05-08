#pragma once

#include <stdexcept>
#include <string>

#include "loader/Loader.hpp"
#include "graphics/Renderer.hpp"
#include "audio/AudioEngine.hpp"
#include "window/Window.hpp"
#include "ui/Userinterface.hpp"
#include "script/ScriptEngine.hpp"
#include "input/Input.hpp"

namespace RIS
{
    struct SystemNotProvidedException : public std::runtime_error
	{
		SystemNotProvidedException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    class SystemLocator
    {
    public:
        void Provide(Window::Window *window);
        void Provide(Graphics::Renderer *renderer);
        void Provide(Audio::AudioEngine *audio);
        void Provide(UI::Userinterface *userinterface);
        void Provide(Loader::Loader *loader);
        void Provide(Input::Input *input);
        void Provide(Script::ScriptEngine *script);

        Window::Window& GetWindow() const;
        Graphics::Renderer& GetRenderer() const;
        Audio::AudioEngine& GetAudio() const;
        UI::Userinterface& GetUserinterface() const;
        Loader::Loader& GetLoader() const;
        Input::Input& GetInput() const;
        Script::ScriptEngine& GetScriptEngine() const;

    private:
        Window::Window *window = nullptr;
        Graphics::Renderer *renderer = nullptr;
        Audio::AudioEngine *audio = nullptr;
        UI::Userinterface *userinterface = nullptr;
        Loader::Loader *loader = nullptr;
        Input::Input *input = nullptr;
        Script::ScriptEngine *script = nullptr;

    };
}

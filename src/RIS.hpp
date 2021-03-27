#pragma once

#include "misc/Config.hpp"
#include "misc/Args.hpp"

namespace RIS
{
    namespace Graphics { class Renderer; }
    namespace UI { class Userinterface; class Console; }
    namespace Audio { class AudioEngine; }
    namespace Window { class Window; }
    //namespace Script { class ScriptEngine; }
    namespace Input { class Input; }

    Graphics::Renderer&     GetRenderer();
    UI::Userinterface&      GetUserinterface();
    UI::Console&            GetConsole();
    Audio::AudioEngine&     GetAudioEngine();
    Window::Window&         GetWindow();
    Input::Input&           GetInput();
    //Script::ScriptEngine&   GetScriptEngine();

    Config& GetConfig();
    const Args& GetArgs();
}

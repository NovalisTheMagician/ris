#include "game/GameLoop.hpp"

#include "RIS.hpp"
#include "window/Window.hpp"
#include "graphics/Renderer.hpp"
#include "audio/AudioEngine.hpp"
#include "ui/Userinterface.hpp"
#include "input/Input.hpp"

#include "window/Paths.hpp"

#include "misc/Timer.hpp"
#include "misc/Logger.hpp"
#include "misc/Config.hpp"

#include <fmt/format.h>

using namespace std::literals;

namespace RIS::Game
{
    GameLoop::GameLoop(Loader::ResourcePack &&resourcePack, std::string_view loadMap)
        : resourcePack(std::move(resourcePack))
        , inputMapper((Window::GetConfigPath() / Input::BINDINGS_FILE_NAME).generic_string())
        , state(LoadScene(loadMap, this->resourcePack))
    {}

    int GameLoop::Start()
    {
        Window::CreatePaths();

        auto &window = GetWindow();
        auto &renderer = GetRenderer();
        auto &input = GetInput();
        auto &interface = GetUserinterface();
        auto &audio = GetAudioEngine();
        auto &console = GetConsole();
        auto &config = GetConfig();

        input.RegisterMouse([this](float x, float y){ inputMapper.OnMouseMove(x, y); return true; }, true);
        input.RegisterButtonDown([this](Input::InputKey button){ inputMapper.OnInputDown(button); return true; }, true);
        input.RegisterButtonUp([this](Input::InputKey button){ inputMapper.OnInputUp(button); return true; }, true);
        input.RegisterWheel([this](float x, float y){ inputMapper.OnMouseWheel(x, y); return true; }, true);
        input.RegisterKeyDown([this](Input::InputKey key){ inputMapper.OnInputDown(key); return true; }, true);
        input.RegisterKeyUp([this](Input::InputKey key){ inputMapper.OnInputUp(key); return true; }, true);

        window.SetRelativeMouse(true);

        InitMenus();
        InitKeyMapping();
        RegisterFuncs();
        
        Timer timer;

        float accumulator = 0.0f;
        float delta = 1.0f / config.GetValue("g_physfps", 144.0f);

        std::visit([&](auto &&s){ s.Start(); }, state);

        while (!window.HandleMessages())
        {
            timer.Update();
            input.Update();
            interface.Update(timer);
            inputMapper.Update();

            auto nextState = std::visit([](auto &&s){ return s.GetNextState(); }, state);
            if(nextState)
            {
                std::visit([](auto &&s){ s.End(); }, state);
                state = std::move(*nextState);
                std::visit([&](auto &&s){ s.Start(); }, state);
            }

            std::visit([this](auto &&s){ s.HandleInput(inputMapper); }, state);

            float frameTime = timer.Delta();
            if(frameTime >= 0.25f)
                frameTime = 0.25f;
            accumulator += frameTime;

            while(accumulator >= delta)
            {
                std::visit([&](auto &&s){ s.Update(timer, delta); }, state);
                accumulator -= delta;
            }

            const float alpha = accumulator / delta;
            std::visit([&](auto &&s){ s.Draw(alpha); }, state);

            interface.Draw();
            window.Present();
        }

        std::visit([](auto &&s){ s.End(); }, state);

        return 0;
    }
}

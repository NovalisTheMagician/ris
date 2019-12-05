#include "GameLoop.hpp"

#include "common/IWindow.hpp"
#include "common/IRenderer.hpp"
#include "common/IInput.hpp"
#include "common/IUserinterface.hpp"
#include "common/IAudio.hpp"
#include "common/INetwork.hpp"

#include "common/Timer.hpp"
#include "common/Logger.hpp"

#include <thread>
#include <mutex>

namespace RIS
{
    GameLoop::GameLoop(const SystemLocator &systems, const Config &config, const Args &args)
        : systems(systems), config(config), args(args)
    {

    }

    GameLoop::~GameLoop()
    {

    }

    int GameLoop::Start()
    {
        StartThreads();

        IWindow &window = systems.GetWindow();
        IRenderer &renderer = systems.GetRenderer();
        IInput &input = systems.GetInput();
        IUserinterface &interface = systems.GetUserinterface();
        IAudio &audio = systems.GetAudio();

        glm::vec4 clearColor(0.392f, 0.584f, 0.929f, 1.0f);
        clearColor = glm::pow(clearColor, glm::vec4(2.2f));

        Timer timer;

        while (!window.HandleMessages())
        {
            timer.Update();
            
            input.Update();

            renderer.Clear(0, clearColor);
            window.Present();
        }

        return 0;
    }

    void GameLoop::StartThreads()
    {

    }
}

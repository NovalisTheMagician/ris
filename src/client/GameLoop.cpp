#include "GameLoop.hpp"

#include "common/IWindow.hpp"
#include "common/IRenderer.hpp"
#include "common/IInput.hpp"
#include "common/IUserinterface.hpp"
#include "common/IAudio.hpp"
#include "common/INetwork.hpp"

#include "common/Timer.hpp"
#include "common/Logger.hpp"

#include "common/StringSupport.hpp"
#include "common/ThreadHelper.hpp"

#include <thread>
#include <atomic>
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
        //clearColor = glm::pow(clearColor, glm::vec4(2.2f));

        Timer timer;

        while (!window.HandleMessages())
        {
            timer.Update();
            input.Update();

            interface.Update(timer);

            renderer.Clear(0, clearColor);
            interface.Draw();
            window.Present();
        }

        runNetworkThread = false;
        networkThread.join();
        runGameplayThread = false;
        gameplayThread.join();

        return 0;
    }

    void GameLoop::StartThreads()
    {
        auto &console = systems.GetUserinterface().GetConsole();

        runNetworkThread = true;
        networkThread = std::thread(std::bind(&GameLoop::NetworkThread, this));
        console.Print("Network Thread-Id: " + toString(networkThread.get_id()));

        runGameplayThread = true;
        gameplayThread = std::thread(std::bind(&GameLoop::GameplayThread, this));
        console.Print("Gameplay Thread-Id: " + toString(gameplayThread.get_id()));
    }

    void GameLoop::NetworkThread()
    {
        INetwork &network = systems.GetNetwork();
        while(runNetworkThread)
        {
            network.StepLoop();
        }
    }

    void GameLoop::GameplayThread()
    {
        Timer timer;
        while(runGameplayThread)
        {
            timer.Update();
        }
    }
}

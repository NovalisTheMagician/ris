#include "GameLoop.hpp"

#include "common/IWindow.hpp"
#include "common/IRenderer.hpp"

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

        glm::vec4 clearColor(0.392f, 0.584f, 0.929f, 1.0f);
        clearColor = glm::pow(clearColor, glm::vec4(2.2f));

        while (!window.HandleMessages())
        {
            renderer.Clear(clearColor);
            window.Present();
        }

        return 0;
    }

    void GameLoop::StartThreads()
    {

    }
}

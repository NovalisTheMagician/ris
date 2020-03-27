#include "GameLoop.hpp"

#include "common/IWindow.hpp"
#include "common/IRenderer.hpp"
#include "common/IInput.hpp"
#include "common/IUserinterface.hpp"
#include "common/IAudio.hpp"
#include "common/INetwork.hpp"
#include "common/LuaScriptEngine.hpp"

#include "common/Timer.hpp"
#include "common/Logger.hpp"

#include "common/StringSupport.hpp"
#include "common/ThreadHelper.hpp"

using namespace std::literals;

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
        IWindow &window = systems.GetWindow();
        IRenderer &renderer = systems.GetRenderer();
        IInput &input = systems.GetInput();
        IUserinterface &interface = systems.GetUserinterface();
        IAudio &audio = systems.GetAudio();
        LuaScriptEngine &scriptEngine = dynamic_cast<LuaScriptEngine&>(systems.GetScriptEngine());

        bool god = false;
        interface.GetConsole().BindFunc("god", Helpers::BoolFunc(god, "Godmode ON", "Godmode OFF"));

        scriptEngine.LoadScript("main");
        scriptEngine.RegisterFunction([](const char *msg){ Logger::Instance().Info("{lua}"s + msg); }, "logger", "info");
        scriptEngine.RegisterFunction([](const char *msg){ Logger::Instance().Warning("{lua}"s + msg); }, "logger", "warning");
        scriptEngine.RegisterFunction([](const char *msg){ Logger::Instance().Error("{lua}"s + msg); }, "logger", "error");

        scriptEngine.CallFunction("", "main", 5);

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

        return 0;
    }
}

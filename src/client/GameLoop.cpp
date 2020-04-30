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

#include <glm/gtc/matrix_transform.hpp>

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
        I3DRenderer &renderer3d = renderer.Get3DRenderer();
        IInput &input = systems.GetInput();
        IUserinterface &interface = systems.GetUserinterface();
        IAudio &audio = systems.GetAudio();
        LuaScriptEngine &scriptEngine = dynamic_cast<LuaScriptEngine&>(systems.GetScriptEngine());

        bool god = false;
        interface.GetConsole().BindFunc("god", Helpers::BoolFunc(god, "Godmode ON", "Godmode OFF"));

        scriptEngine.LoadScript("main");
        scriptEngine.Register([](const char *msg){ Logger::Instance().Info("{lua}"s + msg); }, "logger", "info");
        scriptEngine.Register([](const char *msg){ Logger::Instance().Warning("{lua}"s + msg); }, "logger", "warning");
        scriptEngine.Register([](const char *msg){ Logger::Instance().Error("{lua}"s + msg); }, "logger", "error");

        scriptEngine.CallFunction("", "main");

        ResourceId cubeModel = renderer3d.LoadModel("john");
        //ResourceId catTexture = renderer.LoadTexture("john");
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(5, 5, 5), glm::vec3(), glm::vec3(0, 1, 0));
        glm::mat4 world = glm::mat4(1.0f);

        glm::vec3 camPos(std::cosf(0), 5, std::sinf(0));
        float x = 0;

        glm::vec4 clearColor(0.392f, 0.584f, 0.929f, 1.0f);

        Timer timer;

        while (!window.HandleMessages())
        {
            timer.Update();
            input.Update();

            x += timer.Delta();
            camPos = glm::vec3(std::cosf(x) * 5, 5, std::sinf(x) * 5);

            view = glm::lookAt(camPos, glm::vec3(), glm::vec3(0, 1, 0));

            interface.Update(timer);

            renderer.Clear(0, clearColor);

            renderer3d.Begin(projection * view);
            renderer3d.Draw(cubeModel, world);
            renderer3d.End();

            interface.Draw();
            
            window.Present();
        }

        return 0;
    }
}

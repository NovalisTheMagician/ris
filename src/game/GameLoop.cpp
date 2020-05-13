#include "game/GameLoop.hpp"

#include "RIS.hpp"
#include "window/Window.hpp"
#include "graphics/Renderer.hpp"
#include "audio/AudioEngine.hpp"
#include "ui/Userinterface.hpp"
#include "input/Input.hpp"
#include "script/ScriptEngine.hpp"
#include "loader/Loader.hpp"

#include "misc/Timer.hpp"
#include "misc/Logger.hpp"

#include "graphics/Framebuffer.hpp"

#include "misc/StringSupport.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

using namespace std::literals;

namespace RIS
{
    namespace Game
    {
        int GameLoop::Start()
        {
            auto &window = GetWindow();
            auto &renderer = GetRenderer();
            auto &input = GetInput();
            auto &interface = GetUserinterface();
            auto &audio = GetAudioEngine();
            auto &scriptEngine = GetScriptEngine();
            auto &loader = GetLoader();

            bool god = false;
            interface.GetConsole().BindFunc("god", UI::Helpers::BoolFunc(god, "Godmode ON", "Godmode OFF"));

            scriptEngine.CallFunction<void>("mainTest");

            //std::shared_ptr<Graphics::Model> cubeModel = loader.Load<Graphics::Model>("models/john.glb");
            //ResourceId catTexture = renderer.LoadTexture("john");
            glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(5, 5, 5), glm::vec3(), glm::vec3(0, 1, 0));
            glm::mat4 world = glm::mat4(1.0f);

            glm::vec3 camPos(std::cosf(0), 5, std::sinf(0));
            float x = 0;

            glm::vec4 clearColor(0.392f, 0.584f, 0.929f, 1.0f);

            Timer timer;

            Graphics::Framebuffer defaultFramebuffer;

            while (!window.HandleMessages())
            {
                timer.Update();
                input.Update();

                x += timer.Delta();
                camPos = glm::vec3(std::cosf(x) * 5, 5, std::sinf(x) * 5);

                view = glm::lookAt(camPos, glm::vec3(), glm::vec3(0, 1, 0));

                interface.Update(timer);

                defaultFramebuffer.Clear(clearColor, 1.0f);

                //renderer3d.Begin(projection * view);
                //renderer3d.Draw(cubeModel, world);
                //renderer3d.End();

                interface.Draw();
                
                window.Present();
            }

            return 0;
        }
    }
}

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

#include <iostream>

#include "graphics/SpriteRenderer.hpp"

#include "graphics/Sampler.hpp"
#include "graphics/ProgramPipeline.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/VertexTypes.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Buffer.hpp"

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

            std::shared_ptr<Graphics::Texture> catTexture = loader.Load<Graphics::Texture>("textures/meow.dds");
            std::shared_ptr<Graphics::Font> font = loader.Load<Graphics::Font>("fonts/immortal.json");
            std::shared_ptr<Graphics::Model> cubeModel = loader.Load<Graphics::Model>("models/john.json");

            std::shared_ptr<Graphics::Shader> modelVertexShader = loader.Load<Graphics::Shader>("shaders/mStatic.glsl", Graphics::ShaderType::VERTEX);
            std::shared_ptr<Graphics::Shader> modelFragmentShader = loader.Load<Graphics::Shader>("shaders/mUnlit.glsl", Graphics::ShaderType::FRAGMENT);

            Graphics::VertexArray modelLayout;
            modelLayout.SetAttribFormat(0, 3, GL_FLOAT, offsetof(VertexType::ModelVertex, position));
            modelLayout.SetAttribFormat(1, 3, GL_FLOAT, offsetof(VertexType::ModelVertex, normal));
            modelLayout.SetAttribFormat(2, 2, GL_FLOAT, offsetof(VertexType::ModelVertex, texCoords));
            modelLayout.SetAttribFormat(3, 4, GL_BYTE, offsetof(VertexType::ModelVertex, joints));
            modelLayout.SetAttribFormat(4, 4, GL_FLOAT, offsetof(VertexType::ModelVertex, weights));

            Graphics::ProgramPipeline pipeline;
            pipeline.SetShader(*modelVertexShader);
            pipeline.SetShader(*modelFragmentShader);

            Graphics::Sampler sampler(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 16.0f);

            Graphics::Buffer viewProjBuffer(glm::mat4(), GL_DYNAMIC_STORAGE_BIT);
            Graphics::Buffer worldBuffer(glm::mat4(), GL_DYNAMIC_STORAGE_BIT);

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
                viewProjBuffer.UpdateData(projection * view);
                worldBuffer.UpdateData(world);

                interface.Update(timer);

                defaultFramebuffer.Clear(clearColor, 1.0f);

                pipeline.Use();
                viewProjBuffer.Bind(GL_UNIFORM_BUFFER, 0);
                worldBuffer.Bind(GL_UNIFORM_BUFFER, 1);

                modelLayout.Bind();
                cubeModel->GetMesh()->Bind(modelLayout);
                cubeModel->GetTexture()->Bind(0);
                sampler.Bind(0);
                cubeModel->GetMesh()->Draw();

                interface.Draw();
                
                window.Present();
            }

            return 0;
        }
    }
}

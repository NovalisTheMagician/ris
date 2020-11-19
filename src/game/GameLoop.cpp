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
#include "misc/Config.hpp"

#include "graphics/Framebuffer.hpp"

#include "misc/StringSupport.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/Sampler.hpp"
#include "graphics/ProgramPipeline.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/VertexTypes.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Animation.hpp"
#include "graphics/Transform.hpp"
#include "graphics/Model.hpp"

using namespace std::literals;

namespace RIS
{
    namespace Game
    {
        GameLoop::GameLoop(Loader::ResourcePack &&resourcePack)
            : resourcePack(std::move(resourcePack))
        {}

        int GameLoop::Start()
        {
            auto &window = GetWindow();
            auto &renderer = GetRenderer();
            auto &input = GetInput();
            auto &interface = GetUserinterface();
            auto &audio = GetAudioEngine();
            auto &scriptEngine = GetScriptEngine();

            bool god = false;
            interface.GetConsole().BindFunc("god", UI::Helpers::BoolFunc(god, "Godmode ON", "Godmode OFF"));

            Graphics::Texture::Ptr catTexture = Loader::Load<Graphics::Texture>("textures/meow.dds", resourcePack);
            Graphics::Font::Ptr font = Loader::Load<Graphics::Font>("fonts/immortal.json", resourcePack);
            Graphics::Model::Ptr cubeModel = Loader::Load<Graphics::Model>("models/john.json", resourcePack);

            Graphics::Shader::Ptr modelVertexShader = Loader::Load<Graphics::Shader>("shaders/mAnim.glsl", resourcePack, Graphics::ShaderType::VERTEX);
            Graphics::Shader::Ptr modelFragmentShader = Loader::Load<Graphics::Shader>("shaders/mUnlit.glsl", resourcePack, Graphics::ShaderType::FRAGMENT);

            Graphics::Animation::Skeleton::Ptr skeleton = Loader::Load<Graphics::Animation::Skeleton>("meshes/john.glb", resourcePack);
            Graphics::Animation::Animation::Ptr animation = Loader::Load<Graphics::Animation::Animation>("meshes/john.glb", resourcePack);

            Graphics::VertexArray modelLayout(VertexType::ModelVertexFormat);

            Graphics::ProgramPipeline pipeline;
            pipeline.SetShader(*modelVertexShader);
            pipeline.SetShader(*modelFragmentShader);

            Graphics::Sampler sampler(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 16.0f);

            Graphics::Buffer viewProjBuffer(glm::mat4(), GL_DYNAMIC_STORAGE_BIT);
            Graphics::Buffer worldBuffer(glm::mat4(), GL_DYNAMIC_STORAGE_BIT);

            auto &config = GetConfig();
            float width = config.GetValue("r_width", 800.0f);
            float height = config.GetValue("r_height", 600.0f);

            glm::mat4 projection = glm::perspective(glm::radians(60.0f), width / height, 0.1f, 1000.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(5, 5, 5), glm::vec3(), glm::vec3(0, 1, 0));
            glm::mat4 world = glm::mat4(1.0f);

            float dist = 5;
            interface.GetConsole().BindFunc("dist", [&dist](std::vector<std::string> params)
            {
                if(params.size() > 0)
                    dist = std::stof(params.at(0));
                return "";
            });

            float animSpeed = 1.0f;
            interface.GetConsole().BindFunc("anim_speed", [&animSpeed](std::vector<std::string> params)
            {
                if(params.size() > 0)
                    animSpeed = std::stof(params.at(0));
                return "";
            });

            bool debugDraw = false;
            interface.GetConsole().BindFunc("debug_draw", UI::Helpers::BoolFunc(debugDraw, "Show debug stuff", "Dont show debug stuff"));

            glm::vec3 camPos(std::cos(0.0f) * dist, 5, std::sin(0.0f) * dist);
            float x = 0;

            glm::vec4 clearColor(0.392f, 0.584f, 0.929f, 1.0f);

            Timer timer;

            Graphics::Framebuffer defaultFramebuffer;

            std::vector<glm::mat4> matrixPalette;
            float playbackTime = 0.0f;

            auto &clip = animation->GetByIndex(0);
            Graphics::Animation::Pose &animPose = skeleton->GetBindPose();

            Graphics::Buffer skeletonBuffer(sizeof glm::mat4 * 120, GL_DYNAMIC_STORAGE_BIT);

            const std::vector<glm::mat4> &invBindPose = skeleton->GetInvBindPose();

            clip.SetLooping(true);

#pragma region DebugSetup

            Graphics::Shader::Ptr debugVertexShader = Loader::Load<Graphics::Shader>("shaders/debugVertex.glsl", resourcePack, Graphics::ShaderType::VERTEX);
            Graphics::Shader::Ptr debugFragmentShader = Loader::Load<Graphics::Shader>("shaders/debugFragment.glsl", resourcePack, Graphics::ShaderType::FRAGMENT);

            Graphics::ProgramPipeline debugPipeline;
            debugPipeline.SetShader(*debugVertexShader);
            debugPipeline.SetShader(*debugFragmentShader);

            Graphics::VertexArray debugLayout;
            debugLayout.SetAttribFormat(0, 3, GL_FLOAT, 0);

            std::vector<glm::vec3> points;
            std::vector<glm::vec3> lines;
            for(std::size_t i = 0; i < animPose.Size(); ++i)
            {
                auto transform = animPose.GetGlobalTransform(i);
                points.push_back(transform.position);

                int parent = animPose.GetParent(i);
                if(parent >= 0)
                {
                    auto parentTrans = animPose.GetGlobalTransform(parent);
                    lines.push_back(transform.position);
                    lines.push_back(parentTrans.position);
                }
                else
                {
                    lines.push_back(transform.position);
                    glm::vec3 nextPos = transform.position + (transform.rotation * (transform.scale * glm::vec3(0, 1, 0)));
                    lines.push_back(nextPos);
                }
            }

            Graphics::Buffer bonePointBuffer(points, GL_DYNAMIC_STORAGE_BIT);
            Graphics::Buffer boneLineBuffer(lines, GL_DYNAMIC_STORAGE_BIT);

            glDisable(GL_PROGRAM_POINT_SIZE);
            glPointSize(6.0f);

#pragma endregion DebugSetup

            while (!window.HandleMessages())
            {
                timer.Update();
                input.Update();

                float dt = timer.Delta();

                playbackTime = clip.Sample(animPose, playbackTime + timer.Delta() * animSpeed);
                animPose.GetMatrixPalette(matrixPalette);

                for(std::size_t i = 0; i < matrixPalette.size(); ++i)
                    matrixPalette[i] = matrixPalette[i] * invBindPose[i];

                skeletonBuffer.UpdateData(matrixPalette);

                x += timer.Delta();
                camPos = glm::vec3(std::cos(x) * dist, 5, std::sin(x) * dist);

                view = glm::lookAt(camPos, glm::vec3(), glm::vec3(0, 1, 0));
                viewProjBuffer.UpdateData(projection * view);
                worldBuffer.UpdateData(world);

                interface.Update(timer);

                defaultFramebuffer.Clear(clearColor, 1.0f);

                pipeline.Use();
                viewProjBuffer.Bind(GL_UNIFORM_BUFFER, 0);
                worldBuffer.Bind(GL_UNIFORM_BUFFER, 1);
                skeletonBuffer.Bind(GL_UNIFORM_BUFFER, 2);

                modelLayout.Bind();
                cubeModel->GetMesh()->Bind(modelLayout);
                cubeModel->GetTexture()->Bind(0);
                sampler.Bind(0);
                cubeModel->GetMesh()->Draw();

#pragma region DebugDraw

                if(debugDraw)
                {
                    lines.clear();
                    for(std::size_t i = 0; i < animPose.Size(); ++i)
                    {
                        auto transform = animPose.GetGlobalTransform(i);
                        points[i] = transform.position;

                        int parent = animPose.GetParent(i);
                        if(parent >= 0)
                        {
                            auto parentTrans = animPose.GetGlobalTransform(parent);
                            lines.push_back(transform.position);
                            lines.push_back(parentTrans.position);
                        }
                        else
                        {
                            lines.push_back(transform.position);
                            glm::vec3 nextPos = transform.position + (transform.rotation * (transform.scale * transform.position));
                            lines.push_back(nextPos);
                        }
                    }

                    bonePointBuffer.UpdateData(points);
                    boneLineBuffer.UpdateData(lines);

                    glDisable(GL_DEPTH_TEST);

                    debugPipeline.Use();
                    debugLayout.Bind();
                    debugLayout.SetVertexBuffer<glm::vec3>(bonePointBuffer);
                    glDrawArrays(GL_POINTS, 0, points.size());

                    debugLayout.SetVertexBuffer<glm::vec3>(boneLineBuffer);
                    glDrawArrays(GL_LINES, 0, lines.size());

                    glEnable(GL_DEPTH_TEST);
                }

#pragma endregion DebugDraw

                interface.Draw();
                
                window.Present();
            }

            return 0;
        }
    }
}

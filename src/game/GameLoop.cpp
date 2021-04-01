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
#include "misc/Version.hpp"

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
#include "graphics/Font.hpp"

#include <fmt/format.h>

#include <thread>

#include "window/Paths.hpp"

using namespace std::literals;

namespace RIS::Game
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
        auto &console = interface.GetConsole();

        InitMenus();

        bool god = false;
        console.BindFunc("god", UI::Helpers::BoolFunc(god, "Godmode ON", "Godmode OFF"));

        console.BindFunc("attention", [&window](std::vector<std::string> params){ window.RequestAttention(); return ""; });

        auto test = Loader::Load<std::string>("test", resourcePack);
        if(test)
            console.Print(*test);

        Graphics::Texture::Ptr catTexture = Loader::Load<Graphics::Texture>("textures/meow.dds", resourcePack);
        Graphics::Model::Ptr cubeModel = Loader::Load<Graphics::Model>("models/John.json", resourcePack);

        Graphics::Shader::Ptr modelVertexShader = Loader::Load<Graphics::Shader>("shaders/mAnim.glsl", resourcePack, Graphics::ShaderType::VERTEX);
        Graphics::Shader::Ptr modelFragmentShader = Loader::Load<Graphics::Shader>("shaders/mUnlit.glsl", resourcePack, Graphics::ShaderType::FRAGMENT);

        Graphics::Animation::Skeleton::Ptr skeleton = Loader::Load<Graphics::Animation::Skeleton>("meshes/John.glb", resourcePack);
        Graphics::Animation::Animation::Ptr animation = Loader::Load<Graphics::Animation::Animation>("meshes/John.glb", resourcePack);

        Graphics::VertexArray modelLayout(VertexType::ModelVertexFormat);

        Graphics::ProgramPipeline pipeline;
        pipeline.SetShader(*modelVertexShader);
        pipeline.SetShader(*modelFragmentShader);

        //Graphics::Sampler sampler(Graphics::MinFilter::LINEAR_MIPMAP_LINEAR, Graphics::MagFilter::LINEAR, 16.0f);
        Graphics::Sampler sampler = Graphics::Sampler::Trilinear(16.0f);

        Graphics::UniformBuffer viewProjBuffer(glm::mat4{});
        Graphics::UniformBuffer worldBuffer(glm::mat4{});

        auto &config = GetConfig();
        float width = config.GetValue("r_width", 800.0f);
        float height = config.GetValue("r_height", 600.0f);

        glm::mat4 projection = glm::perspective(glm::radians(60.0f), width / height, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(5, 5, 5), glm::vec3(), glm::vec3(0, 1, 0));
        glm::mat4 world = glm::mat4(1.0f);

        float dist = 5;
        console.BindFunc("dist", [&dist](std::vector<std::string> params)
        {
            if(params.size() > 0)
                dist = std::stof(params.at(0));
            return "";
        });

        float animSpeed = 1.0f;
        console.BindFunc("anim_speed", [&animSpeed](std::vector<std::string> params)
        {
            if(params.size() > 0)
                animSpeed = std::stof(params.at(0));
            return "";
        });

        bool debugDraw = false;
        console.BindFunc("debug_draw", UI::Helpers::BoolFunc(debugDraw, "Show debug stuff", "Dont show debug stuff"));

        glm::vec3 camPos(std::cos(0.0f) * dist, 5, std::sin(0.0f) * dist);
        float x = 0;

        glm::vec4 clearColor(0.392f, 0.584f, 0.929f, 1.0f);

        Timer timer;

        Graphics::Framebuffer defaultFramebuffer;

        std::vector<glm::mat4> matrixPalette;
        float playbackTime = 0.0f;

        auto &clip = animation->GetByIndex(0);
        Graphics::Animation::Pose &animPose = skeleton->GetBindPose();

        Graphics::UniformBuffer skeletonBuffer(sizeof(glm::mat4) * 120);

        const std::vector<glm::mat4> &invBindPose = skeleton->GetInvBindPose();

        clip.SetLooping(true);

        console.Print(Window::GetConfigPath().generic_string());
        console.Print(Window::GetSavePath().generic_string());
        console.Print(Window::GetModPath().generic_string());
        console.Print(Window::GetTempPath().generic_string());

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
            viewProjBuffer.Bind(0);
            worldBuffer.Bind(1);
            skeletonBuffer.Bind(2);

            modelLayout.Bind();
            //cubeModel->GetMesh()->Bind(modelLayout);
            //cubeModel->GetTexture()->Bind(0);
            cubeModel->Bind(modelLayout);
            sampler.Bind(0);
            //cubeModel->GetMesh()->Draw();
            cubeModel->Draw();

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

    void GameLoop::InitMenus()
    {
        std::string version = fmt::format("V {}.{}", std::to_string(Version::MAJOR), std::to_string(Version::MINOR));
        auto &ui = GetUserinterface();

        GetConsole().Print(version);

        float w = ui.GetWidth();
        float h = ui.GetHeight() - 24;

        auto font = Loader::Load<Graphics::Font>("fonts/IMMORTAL.json", resourcePack);

        float titleFontSize = 60;

        auto titleMetrics = font->MeasureString(Version::GAME_NAME, titleFontSize);

        auto titleLabel = UI::Label::Create(font);
        titleLabel->SetName("title");
        titleLabel->SetFontSize(titleFontSize);
        titleLabel->SetText(std::string(Version::GAME_NAME));
        titleLabel->SetPosition({(w / 2.0f) - (titleMetrics.width / 2.0f), titleMetrics.height + 10.0f});

        auto versionMetrics = font->MeasureString(version, 16);

        auto versionLabel = UI::Label::Create(font);
        versionLabel->SetName("version");
        versionLabel->SetFontSize(16);
        versionLabel->SetTextColor({1, 1, 0, 1});
        versionLabel->SetPosition({w - versionMetrics.width, h - versionMetrics.height});
        versionLabel->SetText(version);

        auto catImage = Loader::Load<Graphics::Texture>("textures/meow.dds", resourcePack);

        auto img = UI::Image::Create();
        img->SetName("img");
        img->SetPosition({w - 100, 0});
        img->SetImage(catImage);
        img->SetSize({100, 100});

        auto btn = UI::Button::Create(font);
        btn->SetName("btn1");
        btn->SetPosition({10, h - 10});
        btn->SetSize({74, 24});
        btn->SetText("Quit");
        btn->SetFontSize(16);
        btn->SetCallback([](){ GetWindow().Exit(0); });

        auto btn2 = UI::Button::Create(font);
        btn2->SetName("btn2");
        btn2->SetPosition({10, h - 36});
        btn2->SetSize({74, 24});
        btn2->SetText("Options");
        btn2->SetFontSize(16);
        //btn2->SetCallback([](){});
        btn2->SetActive(false);

        auto textBox = UI::InputBox::Create(font);
        textBox->SetName("txtbox");
        textBox->SetPreviewText("Hello World");
        textBox->SetText("Hah");
        textBox->SetPosition({100, 100});
        textBox->SetSize({128, 32});
        textBox->SetFontSize(16);

        auto btn3 = UI::Button::Create(font);
        btn3->SetName("btn2");
        btn3->SetPosition({10, h - 36 - 26});
        btn3->SetSize({74, 24});
        btn3->SetText("Play");
        btn3->SetFontSize(16);
        btn3->SetCallback([textBox](){ GetConsole().Print(textBox->GetText()); });

        auto panel = UI::Panel::Create();
        panel->SetName("panel");
        panel->Add(img);
        panel->Add(btn);
        panel->Add(btn2);
        panel->Add(btn3);
        panel->Add(titleLabel);
        panel->Add(versionLabel);
        panel->Add(textBox);

        ui.RegisterMenu("mainMenu", panel);
        ui.SetActiveMenu("mainMenu");
    }
}

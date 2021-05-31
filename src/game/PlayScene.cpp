#include "game/State.hpp"

#include "RIS.hpp"
#include "ui/Console.hpp"
#include "window/Window.hpp"

#include "graphics/Framebuffer.hpp"
#include "graphics/Colors.hpp"

#include "misc/Config.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace RIS::Game
{
    PlayScene::PlayScene(SceneData sceneData)
        : sceneData(sceneData)
    {
        auto &config = GetConfig();
        width = config.GetValue("r_width", 800.0f);
        height = config.GetValue("r_height", 600.0f);
    }

    void PlayScene::Start()
    {
        mapLayout = VertexType::MapVertexFormat;
        mapPipeline.SetShader(*sceneData.mapVertexShader);
        mapPipeline.SetShader(*sceneData.mapFragmentShader);

        sampler = Graphics::Sampler::Trilinear(16.0f);
        //sampler = Graphics::Sampler::Nearest(16.0f);

        viewProjBuffer = Graphics::UniformBuffer(glm::mat4{});
        worldBuffer = Graphics::UniformBuffer(glm::mat4{});
    }

    void PlayScene::End()
    {

    }

    void PlayScene::Update(const Timer &timer, float timeStep)
    {
        projection = glm::perspective(glm::radians(75.0f), width / height, 0.1f, 1000.0f);
        view = glm::lookAt(glm::vec3(20, 20, 20), glm::vec3(), glm::vec3(0, 1, 0));
        world = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    }

    void PlayScene::Draw(float interpol)
    {
        Graphics::Framebuffer backbuffer;
        backbuffer.Clear(Graphics::Colors::CornflowerBlue, 1.0f);

        sampler.Bind(0);

        mapPipeline.Use();

        //view = glm::lookAt(camPos, glm::vec3(), glm::vec3(0, 1, 0));
        viewProjBuffer.UpdateData(projection * view);
        worldBuffer.UpdateData(world);

        viewProjBuffer.Bind(0);
        worldBuffer.Bind(1);

        mapLayout.Bind();
        sceneData.mapMesh->Bind(mapLayout);
        sceneData.mapMesh->Draw();
    }

    void PlayScene::OnAction(Input::ActionEvent<Action> e)
    {
        if(e.type == Input::EventType::KEY)
        {
            auto actionEvent = std::get<Input::BinaryEvent<Action>>(e.value);
            switch(actionEvent.action)
            {
            case Action::OPEN_MENU:
                    if(actionEvent.state == Input::EventState::UP)
                        GetConsole().Print("Open Menu");
                break;
            case Action::QUICK_EXIT:
                    if(actionEvent.state == Input::EventState::UP)
                        GetWindow().Exit(0);
                break;
            };
        }
        else if(e.type == Input::EventType::MOUSE_MOVE)
        {

        }
        else //if(e.type == Input::EventType::MOUSE_WHEEL)
        {

        }
    }

    std::optional<State> PlayScene::GetNextState() const
    {
        return std::nullopt;
    }
}
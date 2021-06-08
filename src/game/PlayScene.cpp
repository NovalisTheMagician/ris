#include "game/State.hpp"

#include "RIS.hpp"
#include "ui/Console.hpp"
#include "ui/Userinterface.hpp"
#include "window/Window.hpp"

#include "graphics/Framebuffer.hpp"
#include "graphics/Colors.hpp"

#include "misc/Config.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <fmt/format.h>

namespace RIS::Game
{
    PlayScene::PlayScene(SceneData sceneData, Loader::ResourcePack &resourcePack)
        : sceneData(sceneData), resourcePack(std::ref(resourcePack))
    {
        auto &config = GetConfig();
        width = config.GetValue("r_width", 800.0f);
        height = config.GetValue("r_height", 600.0f);
        float fov = config.GetValue("r_fov", 75.0f);

        camera = Graphics::Camera(glm::radians(fov), width / height);
    }

    void PlayScene::Start()
    {
        GetConsole().BindFunc("map", [this](const std::vector<std::string> &params)
        {
            if(params.size() > 0)
                nextMap = params.at(0);
            return "";
        });

        GetConsole().BindFunc("pos", [this](const std::vector<std::string> &params)
        {
            return fmt::format("{} {} {}", camera.Position().x, camera.Position().y, camera.Position().z);
        });

        mapLayout = VertexType::MapVertexFormat;
        mapPipeline.SetShader(*sceneData.mapVertexShader);
        mapPipeline.SetShader(*sceneData.mapFragmentShader);

        //sampler = Graphics::Sampler::Trilinear(16.0f);
        sampler = Graphics::Sampler::Nearest(16.0f);

        viewProjBuffer = Graphics::UniformBuffer(glm::mat4{});
        worldBuffer = Graphics::UniformBuffer(glm::mat4{});

        camera.Position() = glm::vec3(0, 40, 0); // (-256 -256 40)
        camera.SetYaw(glm::radians(180.0f));
        camera.SetPitch(0);
    }

    void PlayScene::End()
    {
    }

    float speed = 96;

    void PlayScene::HandleInput(const Input::InputMapper<Action> &input)
    {
        camVelocity = glm::vec3(0, 0, 0);
        //camRot = glm::vec2(0, 0);

        if(input.GetState(Action::OPEN_MENU).pressed)
            GetUserinterface().PushMenu("mainMenu");
        if(input.GetState(Action::QUICK_EXIT).pressed)
            GetWindow().Exit(0);

        if(input.GetState(Action::MOVE_FORWARD).held)
            camVelocity.z += speed;
        if(input.GetState(Action::MOVE_BACKWARD).held)
            camVelocity.z -= speed;
        if(input.GetState(Action::STRAFE_RIGHT).held)
            camVelocity.x += speed;
        if(input.GetState(Action::STRAFE_LEFT).held)
            camVelocity.x -= speed;
        if(input.GetState(Action::JUMP).held)
            camVelocity.y += speed;
        if(input.GetState(Action::CROUCH).held)
            camVelocity.y -= speed;

        camRot += -input.GetMouse() * 0.1f;
    }

    float camRadius = 1.0f;

    void PlayScene::Update(const Timer &timer, float timeStep)
    {
        camera.AddYaw(camRot.x * timeStep);
        camera.AddPitch(camRot.y * timeStep);

        glm::vec3 pos = camera.Position();
        glm::vec3 oldPos = camera.Position();

        glm::vec3 movement(0, 0, 0);
        movement += camera.YawDirection() * camVelocity.z * timeStep;
        movement += camera.Right() * camVelocity.x * timeStep;
        movement += glm::vec3(0, 1, 0) * camVelocity.y * timeStep;

        pos.x = sceneData.worldSolids->Collides(oldPos + glm::vec3(movement.x, 0, 0), camRadius) ? oldPos.x : oldPos.x + movement.x;
        pos.y = sceneData.worldSolids->Collides(oldPos + glm::vec3(0, movement.y, 0), camRadius) ? oldPos.y : oldPos.y + movement.y;
        pos.z = sceneData.worldSolids->Collides(oldPos + glm::vec3(0, 0, movement.z), camRadius) ? oldPos.z : oldPos.z + movement.z;

        camera.Position() = pos;

        /*
        glm::vec3 correctedPos = pos;
        if(sceneData.worldSolids->Collides(oldPos, pos, correctedPos))
            camera.Position() = correctedPos;
        else
            camera.Position() = pos;
        */

        world = glm::mat4(1.0f);
        //world = glm::translate(world, glm::vec3(0, -20, 0));
        //world = glm::rotate(world, glm::radians(270.0f), glm::vec3(0, 1, 0));
        //world = glm::scale(world, glm::vec3(0.1f));

        camRot = glm::vec2(0, 0);
    }

    void PlayScene::Draw(float interpol)
    {
        Graphics::Framebuffer backbuffer;
        backbuffer.Clear(Graphics::Colors::CornflowerBlue, 1.0f);

        sampler.Bind(0);

        mapPipeline.Use();

        viewProjBuffer.UpdateData(camera.ViewProj());
        worldBuffer.UpdateData(world);

        viewProjBuffer.Bind(0);
        worldBuffer.Bind(1);

        mapLayout.Bind();
        sceneData.mapMesh->Bind(mapLayout);
        sceneData.mapMesh->Draw();
    }

    std::optional<State> PlayScene::GetNextState() const
    {
        if(!nextMap.empty())
            return LoadScene(nextMap, resourcePack);
        return std::nullopt;
    }
}
#include "game/State.hpp"

#include "RIS.hpp"
#include "ui/Console.hpp"
#include "window/Window.hpp"

#include "graphics/Framebuffer.hpp"
#include "graphics/Colors.hpp"

#include "misc/Config.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <fmt/format.h>

namespace RIS::Game
{
    PlayScene::PlayScene(SceneData sceneData, Loader::ResourcePack &resourcePack, Input::InputMapper<Action> &inputMapper)
        : sceneData(sceneData), resourcePack(std::ref(resourcePack)), inputMapper(std::ref(inputMapper))
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

        GetWindow().SetRelativeMouse(true);

        mapLayout = VertexType::MapVertexFormat;
        mapPipeline.SetShader(*sceneData.mapVertexShader);
        mapPipeline.SetShader(*sceneData.mapFragmentShader);

        sampler = Graphics::Sampler::Trilinear(16.0f);
        //sampler = Graphics::Sampler::Nearest(16.0f);

        viewProjBuffer = Graphics::UniformBuffer(glm::mat4{});
        worldBuffer = Graphics::UniformBuffer(glm::mat4{});

        camera.GetTransform().position = glm::vec3(20, 20, 20);
    }

    void PlayScene::End()
    {
        GetWindow().SetRelativeMouse(false);
    }

    float speed = 64;

    void PlayScene::Update(const Timer &timer, float timeStep)
    {
        glm::vec3 camVelocity(0, 0, 0);
        glm::vec2 camRot(0, 0);

        auto &input = inputMapper.get();
        if(input.GetState(Action::OPEN_MENU).pressed)
            GetConsole().Print("Open Menu");
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

        camRot = -input.GetMouse() * 0.1f;

        camera.AddYaw(camRot.x * timeStep);
        camera.AddPitch(camRot.y * timeStep);

        glm::vec3 pos = camera.GetTransform().position;

        pos += camera.YawDirection() * camVelocity.z * timeStep;
        pos += camera.Right() * camVelocity.x * timeStep;
        pos += glm::vec3(0, 1, 0) * camVelocity.y * timeStep;

        glm::vec3 tmp;
        if(!sceneData.worldSolids->Collides(pos, tmp))
            camera.GetTransform().position = pos;

        world = glm::mat4(1.0f);
        //world = glm::translate(world, glm::vec3(0, -20, 0));
        //world = glm::rotate(world, glm::radians(270.0f), glm::vec3(0, 1, 0));
        //world = glm::scale(world, glm::vec3(0.1f));
    }

    void PlayScene::Draw(float interpol)
    {
        Graphics::Framebuffer backbuffer;
        backbuffer.Clear(Graphics::Colors::CornflowerBlue, 1.0f);

        sampler.Bind(0);

        mapPipeline.Use();

        //view = glm::lookAt(camPos, glm::vec3(), glm::vec3(0, 1, 0));
        viewProjBuffer.UpdateData(camera.ViewProj());
        //viewProjBuffer.UpdateData(projection * view);
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
            return LoadScene(nextMap, resourcePack, inputMapper);
        return std::nullopt;
    }
}
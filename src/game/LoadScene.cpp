#include "game/State.hpp"

#include <fmt/format.h>

#include "RIS.hpp"
#include "ui/Console.hpp"

#include "graphics/Framebuffer.hpp"
#include "graphics/Colors.hpp"

#include "loader/Loaders.hpp"

namespace RIS::Game
{
    LoadScene::LoadScene(std::string_view mapName, Loader::ResourcePack &resourcePack, Input::InputMapper<Action> &inputMapper)
        : resourcePack(std::ref(resourcePack)), inputMapper(std::ref(inputMapper)), doneLoading(false), mapName(mapName)
    {

    }

    void LoadScene::Start()
    {
        sceneData.mapVertexShader = Loader::Load<Graphics::Shader>("shaders/mapVertex.glsl", resourcePack, Graphics::ShaderType::VERTEX);
        sceneData.mapFragmentShader = Loader::Load<Graphics::Shader>("shaders/mapFragment.glsl", resourcePack, Graphics::ShaderType::FRAGMENT);
    }

    void LoadScene::End()
    {
        
    }

    void LoadScene::Update(const Timer &timer, float timeStep)
    {
        if(!sceneData.mapMesh)
        {
            std::string polyFile = fmt::format("{}.poly", mapName);
            std::string solidsFile = fmt::format("{}.brush", mapName);
            sceneData.mapMesh = Loader::Load<Graphics::MapMesh>(polyFile, resourcePack);
            sceneData.worldSolids = Loader::Load<Physics::WorldSolids>(solidsFile, resourcePack);

            doneLoading = true;
        }
    }

    void LoadScene::Draw(float interpol)
    {
        Graphics::Framebuffer backbuffer;

        backbuffer.Clear(Graphics::Colors::Yellow, 1.0f);
    }

    std::optional<State> LoadScene::GetNextState() const
    {
        if(doneLoading)
        {
            Loader::GetCache().Cleanup();
            return PlayScene(sceneData, resourcePack, inputMapper);
        }
        return std::nullopt;
    }
}

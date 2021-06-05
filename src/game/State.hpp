#pragma once

#include <variant>
#include <optional>
#include <string>
#include <string_view>

#include <glm/glm.hpp>

#include "misc/Timer.hpp"
#include "loader/ResourcePack.hpp"

#include "input/InputMapper.hpp"

#include "graphics/MapMesh.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/ProgramPipeline.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Sampler.hpp"
#include "graphics/Camera.hpp"

#include "physics/WorldSolids.hpp"

#include "game/Actions.hpp"
#include "game/MapProps.hpp"

namespace RIS::Game
{
    struct SceneData
    {
        Graphics::MapMesh::Ptr mapMesh;
        MapProps mapProps;
        Physics::WorldSolids::Ptr worldSolids;

        Graphics::Shader::Ptr mapVertexShader;
        Graphics::Shader::Ptr mapFragmentShader;
    };

    class LoadScene;
    class PlayScene;
    using State = std::variant<LoadScene, PlayScene>;

    class PlayScene
    {
    public:
        PlayScene(SceneData sceneData, Loader::ResourcePack &resourcePack, Input::InputMapper<Action> &inputMapper);

        void Start();
        void End();
        void Update(const Timer &timer, float timeStep);
        void Draw(float interpol);

        std::optional<State> GetNextState() const;

    private:
        SceneData sceneData;
        std::reference_wrapper<Loader::ResourcePack> resourcePack;
        std::reference_wrapper<Input::InputMapper<Action>> inputMapper;

        float width, height;

        Graphics::VertexArray mapLayout;
        Graphics::ProgramPipeline mapPipeline;

        Graphics::UniformBuffer viewProjBuffer;
        Graphics::UniformBuffer worldBuffer;

        Graphics::Sampler sampler;

        glm::mat4 world;

        Graphics::Camera camera;

        std::string nextMap;

    };

    class LoadScene
    {
    public:
        LoadScene(std::string_view mapName, Loader::ResourcePack &resourcePack, Input::InputMapper<Action> &inputMapper);

        void Start();
        void End();
        void Update(const Timer &timer, float timeStep);
        void Draw(float interpol);

        std::optional<State> GetNextState() const;

    private:
        std::reference_wrapper<Loader::ResourcePack> resourcePack;
        std::reference_wrapper<Input::InputMapper<Action>> inputMapper;
        bool doneLoading;
        std::string mapName;
        SceneData sceneData;

    };
}

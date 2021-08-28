#pragma once

#include <variant>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

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
#include "game/MapEntity.hpp"

namespace RIS::Game
{
    struct SceneData
    {
        Graphics::MapMesh::Ptr mapMesh;
        MapEntitiesPtr mapEntities;
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
        PlayScene(SceneData sceneData, Loader::ResourcePack &resourcePack);

        void Start();
        void End();
        void HandleInput(const Input::InputMapper<Action> &inputMapper);
        void Update(const Timer &timer, float timeStep);
        void Draw(float interpol);

        std::optional<State> GetNextState() const;

    private:
        SceneData sceneData;
        std::reference_wrapper<Loader::ResourcePack> resourcePack;

        float width, height;

        Graphics::VertexArray mapLayout;
        Graphics::ProgramPipeline mapPipeline;

        Graphics::UniformBuffer viewProjBuffer;
        Graphics::UniformBuffer worldBuffer;

        Graphics::Sampler sampler;

        glm::mat4 world;

        Graphics::Camera camera;

        glm::vec3 camVelocity;
        glm::vec2 camRot;

        std::string nextMap;

    };

    class LoadScene
    {
    public:
        LoadScene(std::string_view mapName, Loader::ResourcePack &resourcePack);

        void Start();
        void End();
        void HandleInput(const Input::InputMapper<Action> &inputMapper);
        void Update(const Timer &timer, float timeStep);
        void Draw(float interpol);

        std::optional<State> GetNextState() const;

    private:
        std::reference_wrapper<Loader::ResourcePack> resourcePack;
        bool doneLoading;
        std::string mapName;
        SceneData sceneData;

    };
}

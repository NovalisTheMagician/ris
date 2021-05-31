#pragma once

#include <variant>
#include <optional>
#include <string>
#include <string_view>

#include <glm/glm.hpp>

#include "misc/Timer.hpp"
#include "loader/ResourcePack.hpp"

#include "input/ActionEvent.hpp"

#include "graphics/MapMesh.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/ProgramPipeline.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Sampler.hpp"

#include "game/Actions.hpp"
#include "game/MapProps.hpp"

namespace RIS::Game
{
    struct SceneData
    {
        Graphics::MapMesh::Ptr mapMesh;
        MapProps mapProps;
        Graphics::Shader::Ptr mapVertexShader;
        Graphics::Shader::Ptr mapFragmentShader;
    };

    class LoadScene;
    class PlayScene;
    using State = std::variant<LoadScene, PlayScene>;

    class PlayScene
    {
    public:
        PlayScene(SceneData sceneData);

        void Start();
        void End();
        void Update(const Timer &timer, float timeStep);
        void Draw(float interpol);

        void OnAction(Input::ActionEvent<Action> e);

        std::optional<State> GetNextState() const;

    private:
        SceneData sceneData;

        float width, height;

        Graphics::VertexArray mapLayout;
        Graphics::ProgramPipeline mapPipeline;

        Graphics::UniformBuffer viewProjBuffer;
        Graphics::UniformBuffer worldBuffer;

        Graphics::Sampler sampler;

        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 world;

    };

    class LoadScene
    {
    public:
        LoadScene(std::string_view mapName, Loader::ResourcePack &resourcePack);

        void Start();
        void End();
        void Update(const Timer &timer, float timeStep);
        void Draw(float interpol);

        void OnAction(Input::ActionEvent<Action> e);

        std::optional<State> GetNextState() const;

    private:
        std::reference_wrapper<Loader::ResourcePack> resourcePack;
        bool doneLoading;
        std::string_view mapName;
        SceneData sceneData;

    };
}
